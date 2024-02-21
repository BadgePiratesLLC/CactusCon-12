#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <freertos/queue.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertising.h>
#include <esp_bt.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "mbedtls/base64.h"

struct BadgeAdvertisement{
    char hmac[20];
    char mac[6];
    int tick;
};

typedef std::function<void(uint8_t *hmac, uint8_t *mac, uint16_t tick)> BadgeDiscoveryCallbackPtr;

class BLE {
    private:
        static esp_ble_gap_periodic_adv_params_t periodic_adv_params;
        static BLEMultiAdvertising advert;
        static BLEScan *pBLEScan;
        static bool advertising;
    public:
        static void setup();
        static void start_scanning();
        static void stop_scanning();
        static bool check_scan_results(char *buf, int buf_size);
        static void start_advertising();
        static void stop_advertising();
        static void start_advertisement(uint8_t *advertisment, uint16_t tick);
        static QueueHandle_t scanning_queue;
};