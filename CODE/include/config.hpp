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
#define OTA_WIFI_SSID "BadgePirates"
#define OTA_WIFI_PASSWORD "APirateIsFr33"
#define OTA_SERVER_URL "http://discord.gr/"
#define OTA_MANIFEST_NAME "version"
#define OTA_BINARY_NAME "firmware.bin" // Don't make this firmware-version-X.bin, this shouldn't need to be changed.