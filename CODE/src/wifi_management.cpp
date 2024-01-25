#include "wifi_management.hpp"

void WifiManagement::setup() {
    WiFi.onEvent(WifiManagement::on_wifi_connect, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(WifiManagement::on_wifi_lost, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

/**
 * @brief This is only here temporarily it will need to be moved to a wifi management/setup class later.
*/
void WifiManagement::get_network_info(){
    if(WiFi.status() == WL_CONNECTED) {
        // format string vulns...
        printf("[+] BSSID : %s\n", ((String) WiFi.BSSIDstr()).c_str());
        printf("[+] Gateway IP : %s\n", (String) WiFi.gatewayIP());
        printf("[+] Subnet Mask : %s\n", (String) WiFi.subnetMask());
        printf("[+] RSSI : %d dB\n", WiFi.RSSI());
        printf("[+] ESP32 IP : %s\n", (String) WiFi.localIP());
        printf("[+] DNS : %s\n", (String) WiFi.dnsIP());
        printf("[+] MAC : %s\n", ((String) WiFi.macAddress()).c_str());
    }
}

/**
 * @brief This is only here temporarily it will need to be moved to a wifi management/setup class later.
*/
void WifiManagement::connect(){
    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);
    WiFi.begin(OTA_WIFI_SSID, OTA_WIFI_PASSWORD);
    printf("[Wifi] Connecting to WiFi");

    for (int i = 0; i < 25 && WiFi.status() != WL_CONNECTED; i++){
        printf(".");
        delay(1000);
    }

    if (WiFi.status() != WL_CONNECTED) {
        WiFi.disconnect();
        printf(" failed!\n");
        ESP.restart();
    }
    printf("connected!");
    //TeleTrack::set_uplink_light(true);
}

void WifiManagement::on_wifi_connect(WiFiEvent_t event, WiFiEventInfo_t info){
    TeleTrack::set_uplink_light(true);
}

void WifiManagement::on_wifi_lost(WiFiEvent_t event, WiFiEventInfo_t info){
    TeleTrack::set_uplink_light(false);
}