#pragma once
/*
* BadgePirates configuration
* Add project-specific configurations to constants.h
* This file should be used to enable and configure common features shared between badges.
*
* For reference, use #define for things constant and used seldomly(because we can save memory with pointers). Hence it should be used for things like 
* Version numbers or OTA WiFi credentials.
*/

#define VERSION 0
#define DEBUG // Comment this out to disable debugging mode
#define BLE_MODE
#define FACTORY_MODE // Comment this out to disable factory mode, which unless you removed the calls in main.cpp, it simply runs that when setup and loop are called.

// OTA bits
#define OTA_SERVER_URL "http://badge.cactuscon.com/ota/"
#define OTA_MANIFEST_NAME "updates"
#define OTA_BINARY_NAME "cactu.bin" // Don't make this firmware-version-X.bin, this shouldn't need to be changed.

const char OTA_WIFI_SSIDS[][255] = {"CactusCon", "Default"};
const char OTA_WIFI_PASSWORDS[][255] = {"Cactus2024!", "Default"};
