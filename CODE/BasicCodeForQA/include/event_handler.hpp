#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "mbedtls/base64.h"
#include "ble.hpp"
#include "teletrack.hpp"

/**
 * @brief Provides callbacks used for handling various events that can occur in the cc12 badge platform.
*/
class EventHandler{
        private:
            static void handle_indicator_message(byte* payload, int length);
            static void handle_badge_message(byte* payload, int length);
        public:
            static void on_message(const char* topic, byte* payload, unsigned int length);
};