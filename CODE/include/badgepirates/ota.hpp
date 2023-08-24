#pragma once

#include <Arduino.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <WiFi.h>

#include "FastLedInclude.hpp"
#include "config.hpp"

class OTA{
    private:
        static bool isWifiAvailable();

        // Progess indicators
        static void indicate_start();
        static void indicate_wifi_connected();
        static void indicate_manifest_load();
        static void indicate_updating();

        // Error indicators
        static void indicate_error_wifi_unavailable();
        static void indicate_error_wifi_rejected(); // PW Fail or uncommon connection hiccup
        static void indicate_error_download();
        static void indicate_error_latest_version();

    public:
        static void checkOTASync();
};