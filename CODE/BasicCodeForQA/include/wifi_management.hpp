#include <Arduino.h>
#include <WiFi.h>
#include "config.hpp"
#include "teletrack.hpp"

class WifiManagement {
    public:
        static void setup();
        static void get_network_info();
        static void connect();
        static void on_wifi_lost(WiFiEvent_t event, WiFiEventInfo_t info);
        static void on_wifi_connect(WiFiEvent_t event, WiFiEventInfo_t info);
};