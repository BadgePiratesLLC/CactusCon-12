#include "aws_mqtt.hpp"

AWSDeviceCredentials device_creds;
AWSMQTTConfiguration device_config;
WiFiClientSecure *secure_client = new WiFiClientSecure;
PubSubClient mqtt_client = PubSubClient(*secure_client);

bool AWSMQTTClient::connect() {

    for (int i = 0; i < 25 && !mqtt_client.connect(device_config.aws_id.c_str(), NULL,NULL,0,0,0,0,0); i++) {
        printf("[AWSMQTT] Failed connect, retrying.\n");
        mqtt_client.disconnect();
        delay(500);
    } 

    if (!mqtt_client.connected()) {
        printf("[AWSMQTT] Connect to MQTT failed!\n");
        ESP.restart();
        return false;
    }
    printf("[AWSMQTT] MQTT connected as %s\n", device_config.aws_id.c_str());
    return true;
}

void AWSMQTTClient::setup(AWSMQTTConfiguration &aws_config, AWSDeviceCredentials &aws_creds) {
    device_creds = aws_creds;
    device_config = aws_config;
    mqtt_client.setServer(AWS_IOT_ENDPOINT, 8883);
    secure_client->setCACert(AWS_PUBLIC_CERT);
    secure_client->setCertificate(device_creds.device_cert.c_str());
    secure_client->setPrivateKey(device_creds.device_key.c_str());
}

bool AWSMQTTClient::start() {
    connect();

    mqtt_client.setCallback(&EventHandler::on_message);
    if (!mqtt_client.subscribe("global/indicators", 0)) {
        printf("[AWSMQTT] Failed subscribe to global/indicators!");
        delay(1000);
        return false;
    }

    if (!mqtt_client.subscribe(device_config.badge_topic.c_str(), 0)){
        printf("[AWSMQTT] Failed subscribe to %s\n", device_config.badge_topic);
        delay(1000);
        return false;
    }
    Serial.printf("[AWSMQTT] Subscribed to %s\n", device_config.badge_topic.c_str());
    return true;
}


bool AWSMQTTClient::publish(char *topic, char *payload) {
    Serial.printf("[AWSMQTT] Publishing advertisement");
    return mqtt_client.publish(topic, payload);    
}

void AWSMQTTClient::loop() {
    mqtt_client.loop();
    delay(10);
    
    if (!mqtt_client.connected()) {
        printf("[AWSMQTT] Lost connection, reconnecting!\n");
        mqtt_client.disconnect();
        connect();
        delay(1000);
    }
}