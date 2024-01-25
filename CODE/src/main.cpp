#include <Arduino.h>
#include <esp_wifi.h>
#include <freertos/queue.h>
#include <Adafruit_NeoPixel.h>

#include "config.hpp"
#include "ota.hpp"
#include "aws_registration.hpp"
#include "aws_mqtt.hpp"
#include "teletrack.hpp"
#include "ble.hpp"
#include "wifi_management.hpp"

// Define the pins for the LEDs
#define LED_1       10
#define LED_2       11
#define LED_3       12
#define LED_4       13
#define LED_5       14
#define LED_6       15
#define LED_PIN     18  // Set to your specific pin
#define NUM_LEDS    3   // Number of LEDs in your strip

QueueHandle_t queue;

const int ota_button_pin = GPIO_NUM_0; // Boot button
const int center_button_pin = GPIO_NUM_36; // Center button
    
AWSMQTTClient mqtt = AWSMQTTClient();
String device_id = WiFi.macAddress();
String badge_topic = "INGSOC/citizen/";
String report_topic = "INGSOC/monitoring";
String aws_id = "cc12-";

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    //init_console();
    Serial.begin(115200);
    
    // Initialize all pixels to 'off'
    strip.begin();
    strip.show();  

    // Lower the power consumption without much impact to performance/usability
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
    setCpuFrequencyMhz(80);
  
    TeleTrack::setup();
    BLE::setup();

    // generate badge and topic IDs
    device_id.replace(":", ""); // Because colons in a MAC string make it smell
    aws_id.concat(device_id);
    badge_topic.concat(aws_id);
    
    // set mqtt params
    AWSMQTTConfiguration aws_config;
    aws_config.device_id = device_id;
    aws_config.aws_id = aws_id;
    aws_config.badge_topic = badge_topic;

    pinMode(ota_button_pin, INPUT_PULLUP);
    pinMode(center_button_pin, INPUT_PULLUP);

    delay(3000);
    
    // Check for buttons held at boot
    if (!digitalRead(ota_button_pin)) { // If OTA update is requested, do this first.
        printf("[Main] Starting OTA update\n");
        OTA::checkOTASync();
    }

    if (!digitalRead(center_button_pin)) { // Reset stored credentials
        printf("[Main] Resetting stored credentials!\n");
        AWSRegistration::clear_saved_credentials();
    }

    WifiManagement::setup();
    WifiManagement::connect();
    WifiManagement::get_network_info();

    if (!AWSRegistration::device_credentials_exist()) { // Check if AWS IOT creds exist
        printf("[Main] Registering device.\n");
        bool success = AWSRegistration::register_new_device(device_id.c_str());

        if (!success) { 
            printf("[Main] Reg failed. Restarting.\n");
        } else {
            printf("[Main] Reg success. Restarting.\n");
        }

        delay(1000);
        ESP.restart();
    }

    AWSDeviceCredentials aws_creds;
    AWSRegistration::load_device_credentials(aws_creds);

    mqtt.setup(aws_config, aws_creds);
    if (!mqtt.start()) {
        delay(1000);
        ESP.restart();
    }
    BLE::start_scanning();

  // Set the additional LEDs as OUTPUT
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);
  pinMode(LED_6, OUTPUT);

}

void loop() {
     // Set random colors for NeoPixel LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    int red = random(256);    // Generate random values for red, green, and blue
    int green = random(256);
    int blue = random(256);

  strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();

  // Blink the additional LEDs randomly
  digitalWrite(LED_1, random(2)); // Randomly set LED_1 on/off
  digitalWrite(LED_2, random(2)); // Randomly set LED_2 on/off
  digitalWrite(LED_3, random(2)); // Randomly set LED_3 on/off
  digitalWrite(LED_4, random(2)); // Randomly set LED_4 on/off
  digitalWrite(LED_5, random(2)); // Randomly set LED_5 on/off
  digitalWrite(LED_6, random(2)); // Randomly set LED_6 on/off

  delay(500);  // Pause between patterns and TFT messages

    char buf[200];

    mqtt.loop();
    if (BLE::check_scan_results(buf, 200)){
        Serial.println(buf);
        mqtt.publish((char *)report_topic.c_str(), buf);
    }


}