#include "event_handler.hpp"

/**
 * @brief Handles a message recieved on the global topic intended for all badges.
 * @param[in] topic A C-style string of the topic the message came in on.
 * @param[in] payload Byte array of the payload of the message.
 * @param[in] length Length in bytes of the message.
*/
void EventHandler::handle_indicator_message(byte* payload, int length) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);

}

/**
 * @brief Handles a message recieved for this specific badge.
 * @param[in] topic A C-style string of the topic the message came in on.
 * @param[in] payload Byte array of the payload of the message.
 * @param[in] length Length in bytes of the message.
*/
void EventHandler::handle_badge_message(byte* payload, int length) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);
    unsigned char output[160];
    const char * data;
    const char * response_status;
    const char * response_message;
    size_t outlen;
    size_t inlen;
    int decode_result;

    if (doc["hmac"].is<const char *>() && doc["tick"].is<uint16_t>()) {
        data = doc["hmac"].as<const char *>();
        inlen = strlen(data); // probably not safe, this will be a bonus esp32 stack smashing exercise
        decode_result = mbedtls_base64_decode(output, 160, &outlen, (unsigned char *)data, inlen);

        // TODO add a signature check here for added safety

        printf("[EventHandler] Restarting advertising with new hmac %s\n", doc["hmac"].as<const char *>());
        //BLE::stop_advertising();
        printf("[EventHandler] Got new advert (len %d %d and res %d)... ", outlen, inlen, decode_result);
        for (int i = 0; i < outlen; i++) {
            printf("%x:", output[i]);
        }
        printf("\n");
        BLE::start_advertisement(output, doc["tick"].as<uint16_t>());
    } 
    if (doc["score"].is<int>()) {
        TeleTrack::set_score(doc["score"]);
    }
    if (doc["status"].is<const char*>()) {
        response_status = doc["status"].as<const char *>();
        response_message = doc["message"].as<const char *>();
        printf("[EventHandler] Server response <%s>: %s\n", response_status, response_message);
    }
}

/**
 * @brief Handles a new message event and routes to the appropriate handler.
 * @param[in] topic A C-style string of the topic the message came in on.
 * @param[in] payload Byte array of the payload of the message.
 * @param[in] length Length in bytes of the message.
*/
void EventHandler::on_message(const char* topic, byte* payload, unsigned int length) {
    String badge_topic = "INGSOC/citizen/cc12-";
    char global_topic[] = "global/indicators";
    int global_topic_len = strlen(global_topic);
    String device_id = WiFi.macAddress();
    device_id.replace(":", "");

    badge_topic.concat(device_id); 

    int badge_topic_len = badge_topic.length();

    // not the safest, but not sure of other options given the limits of the callback signature
    int topic_len = strlen(topic);
    int max_cmp_len = (topic_len > global_topic_len) ? global_topic_len : topic_len;

    Serial.printf("[EventHandler] Got a message on topic %s\n", topic);

    bool is_global = !strncmp(topic,"global/indicators", 
                                (topic_len > global_topic_len) ? global_topic_len : topic_len);
    bool is_badge = !strncmp(topic, badge_topic.c_str(), 
                                (topic_len > badge_topic_len) ? badge_topic_len : topic_len);

    printf("is device %i %s\n", is_badge, badge_topic.c_str());
    if (is_global)
        EventHandler::handle_indicator_message(payload, length);
    if (is_badge)
        EventHandler::handle_badge_message(payload, length);
}