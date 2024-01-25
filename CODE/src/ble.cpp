#include "ble.hpp"

BLEScan* BLE::pBLEScan;

esp_ble_gap_ext_adv_params_t ext_adv_params_2M = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED,
    .interval_min = 0x600,
    .interval_max = 0x2000,
    .channel_map = ADV_CHNL_ALL,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .peer_addr_type = BLE_ADDR_TYPE_RANDOM,
    .peer_addr = {0,0,0,0,0,0},
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .tx_power = -12,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_2M,
    .sid = 1,
    .scan_req_notif = false,
};

uint8_t addr_2m[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

BLEMultiAdvertising BLE::advert(1);

bool BLE::advertising = false;
QueueHandle_t BLE::scanning_queue = NULL;

class MyBLEExtAdvertisingCallbacks: public BLEExtAdvertisingCallbacks {
    void onResult(esp_ble_gap_ext_adv_reprot_t report) {
        uint8_t message[35];
        if(report.event_type == ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED){
            int i;
            Serial.printf("Ext advert mac: ");
            for (i = 0; i < 6; i++)
                Serial.printf("%02X", &report.addr[i]);
            Serial.printf(" , data_le: %d, data_status: %d rssi: %d\n", report.adv_data_len, report.data_status, report.rssi);

            if (report.adv_data_len != 31)
                return;

            if (!(report.adv_data[0] == 'C' && report.adv_data[1] == 'C' && report.adv_data[2] == 0x12))
                return; // not a CC badge
        
            memcpy(message, (void *)&report.rssi, 1);
            memcpy(&message[1], &report.adv_data[3], 28);
            memcpy(&message[29], &report.addr[0], 6);

            // TODO: Extract and use MAC from BLE frame so the 6 bytes can be used for something else... mac_type is 1
            xQueueSend(BLE::scanning_queue, (void*)message, (TickType_t)0);
        }
    }
};

void BLE::setup() {
    BLEDevice::init("");
    BLEDevice::setPower(esp_power_level_t::ESP_PWR_LVL_N12, esp_ble_power_type_t::ESP_BLE_PWR_TYPE_ADV);
    BLE::pBLEScan = BLEDevice::getScan(); //create new scan
    BLE::scanning_queue = xQueueCreate(100, 35);
}

/**
 * @brief Starts a new task to advertise to other badges over BLE
 * @param[in] advertisement pointer to byte array that contains the hmac to advertise.
 * @param[in] tick current tick cycle for the game.
*/
void BLE::start_advertisement(uint8_t * advertisment, uint16_t tick) {
    /** Advertisement format 
     * 0........2 | 3..............................22 |        23         | 24.....25 | 26...
     * CC12 Magic | HMAC advertisement for this badge | WiFi MAC 1st byte | game tick | unassigned...
    */
    uint8_t data[31] = {'C', 'C', 0x12};
    char* ptr;
    uint8_t mac_first_byte[1];
    uint8_t mac[6];

    // Shove the CC12 prefix in to the payload first
    memcpy(&data[3], advertisment, 20);

    // Convert mac string into bytes, could be it's own function
    mac[0] = strtol(WiFi.macAddress().c_str(), &ptr, HEX );
    for( uint8_t i = 1; i < 6; i++ )
        mac[i] = strtol(ptr+1, &ptr, HEX );

    // copy off the first byte for the advert
    mac_first_byte[0] = mac[0];
    // format the mac to meet random address requirements
    mac[0] |= 0xC0;

    // Copy in the mac address as bytes into the payload
    memcpy(&data[23], mac_first_byte, sizeof(mac_first_byte));
    memcpy(addr_2m, mac, sizeof(mac));

    // Convert the tick into bytes and stick in the end of the payload
    data[24] = (byte)(tick >> 8);
    data[25] = (byte)(tick);

    if (BLE::advertising)
        BLE::stop_advertising();

    Serial.printf("Advertisement: ");
    int i;
    for (i = 0; i < 31; i++)
    {
        if (i > 0) printf(":");
        Serial.printf("%02X", data[i]);
    }
    Serial.printf("\n");

    BLE::advert.setAdvertisingParams(0, &ext_adv_params_2M);
    BLE::advert.setAdvertisingData(0, sizeof(data), (uint8_t *)&data[0]);
    BLE::advert.setInstanceAddress(0, addr_2m);
    BLE::advert.setDuration(0, 0, 0);
    BLE::advert.start();
    BLE::advertising = true;
}

void BLE::stop_advertising() {
    BLE::advert.stop(0, 0);
    BLE::advertising = false;
}

/**
 * @brief Starts a new task to scan BLE for CC12 badges.
*/
void BLE::start_scanning() {
    pBLEScan->setExtendedScanCallback(new MyBLEExtAdvertisingCallbacks());
    BLE::pBLEScan->setExtScanParams(); // use with pre-defined/default values, overloaded function allows to pass parameters
    BLE::pBLEScan->setActiveScan(false);
    delay(1000); // it is just for simplicity this example, to let ble stack to set extended scan params
    BLE::pBLEScan->startExtScan(100, 3); // scan duration in n * 10ms, period - repeat after n seconds (period >= duration)
}

/**
 * @brief Checks to see if any new CC12 BLE messages have been discovered. Copies a JSON string into the provided buffer.
 * @param[in] buf A pointer to a C-style string to populate with data.
 * @param[in] buf_size Size of the buffer.
 * @return A bool that indicates whether a message was found.
*/
bool BLE::check_scan_results(char *buf, int buf_size) {
    StaticJsonDocument<200> doc;
    uint8_t ble_message[35];
    uint8_t mac[6];
    uint8_t hmac[20];
    uint8_t tick_b[2];
    int8_t rssi;
    uint16_t tick;
    size_t outlen;
    unsigned char hmac_base64[100];
    unsigned char mac_base64[30];
    char local_buf[200];

    if (xQueueReceive(BLE::scanning_queue, &(ble_message), (TickType_t)5)) {
        // the first 3 bytes are skipped when adding to the queue, but
        // we add rssi, so offset is -2 from raw
        memcpy(&rssi, &ble_message[0], 1);
        memcpy(hmac, &ble_message[1], 20);
        memcpy(mac, &ble_message[21], 1);
        memcpy(&tick_b, &ble_message[22], 2);
        // skip the first byte that's been masked to reconstruct original
        memcpy(&mac[1], &ble_message[30], 5);

        tick = tick_b[0] << 8 | tick_b[1];

        mbedtls_base64_encode(hmac_base64, 100, &outlen, hmac, sizeof(hmac));
	    hmac_base64[outlen] = '\0';
        mbedtls_base64_encode(mac_base64, 30, &outlen, mac, sizeof(mac));
        mac_base64[outlen] = '\0';
        
        doc["advert"] = hmac_base64;
        doc["mac"] = mac_base64;
        doc["tick"] = tick;
        doc["rssi"] = rssi;

        serializeJson(doc, local_buf, buf_size);
        strncpy(buf, local_buf, buf_size);
        return true;
    }

    return false;
}