#include "badgepirates/ota.hpp"
/*
 * BadgePirates OTA updater
 * Handles WiFi updates
 */

// The code here will change badge-to-badge
#pragma region Indication Code
// Progess indicators, should be LED/Screen only.

void OTA::indicate_start(){
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Green;
        FastLED.show();
        delay(30);
    }
}

void OTA::indicate_wifi_connected(){
    FastLED.setBrightness(30);
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Blue;
        FastLED.show();
        delay(30);
    }
}

void OTA::indicate_manifest_load(){
    FastLED.setBrightness(60);
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Black;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Blue;
        FastLED.show();
        delay(30);
    }
}

void OTA::indicate_updating(){
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Purple;
        FastLED.show();
        delay(30);
    }
}

// Error indicators
void OTA::indicate_error_wifi_unavailable(){
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Red;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Blue;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Red;
        FastLED.show();
        delay(30);
    }
}

void OTA::indicate_error_wifi_rejected(){
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Red;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::White;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Red;
        FastLED.show();
        delay(30);
    }
}

void OTA::indicate_error_download(){
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Red;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Orange;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Red;
        FastLED.show();
        delay(30);
    }
}

void OTA::indicate_error_latest_version(){
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Green;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Blue;
        FastLED.show();
        delay(30);
    }
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::White;
        FastLED.show();
        delay(30);
    }
}
#pragma endregion

#pragma region OTA Code
// Checks whether or not our wifi can be seen
bool OTA::isWifiAvailable()
{
    FastLED.setBrightness(75);
    Serial.println("[OTA] WiFi Scan init.");
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks();

#ifdef DEBUG
    Serial.print("[OTA] ");
    Serial.print(n);
    Serial.println(" networks found");
#endif

    if (n == 0)
    {
#ifdef DEBUG
        Serial.println("[OTA] found no wifi networks, please check board.");
#endif
    }
    else
    {
        for (int k = 0; k < n; ++k)
        {
            if (WiFi.SSID(k) == OTA_WIFI_SSID)
            {
                return true;
            }
        }
    }

    return false;
}

void OTA::checkOTASync()
{

    // Since this is a task, we don't get to use the memory of the OTA class.
    HTTPClient httpClient;

    // Start
    indicate_start();
    // Check wifi
    if (!isWifiAvailable())
    {
        indicate_error_wifi_unavailable();
        Serial.println("[OTA] Wifi wasn't found.");
        delay(10000);
        ESP.restart();
    }

    // Connect wifi
    WiFi.persistent(false); // Don't auto-connect on boot
    WiFi.begin(OTA_WIFI_SSID, OTA_WIFI_PASSWORD);
    Serial.print("[OTA] WiFi is connecting");
    for (int i = 0; i < 25 && WiFi.status() != WL_CONNECTED; i++)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\n");

    // Check if wifi failed to connect
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("[OTA] WiFi failed to connect.");

        WiFi.disconnect();
        indicate_error_wifi_rejected();

        delay(10000);
        ESP.restart();
        return;
    }

    Serial.print("[OTA] WiFi connected.");

    // S1 - WiFi Connected
    indicate_wifi_connected();

    // Check update server
    // Try to download the manifest(pull file version, containing an int)
    char urlbuf[50];
    strcpy(urlbuf, OTA_SERVER_URL);
    strcat(urlbuf, OTA_MANIFEST_NAME);
    httpClient.begin(urlbuf);
    int httpCode = httpClient.GET();

    if (httpCode == 200)
    {
        // I - Manifest downloaded
        indicate_manifest_load();

        // Check
        String newFWVersion = httpClient.getString();
        Serial.print("[OTA] Current firmware version: ");
        Serial.println(VERSION);
        Serial.print("[OTA] Available firmware version: ");
        Serial.println(newFWVersion);

        int newVersion = newFWVersion.toInt();
        if (newVersion > VERSION)
        {
            // D - Downloading update
            indicate_updating();

            Serial.println("[OTA] Update is downloading/installing...");
            Serial.println("[OTA] Do not power off the badge");
            char binbuf[50];
            strcpy(binbuf, OTA_SERVER_URL);
            strcat(binbuf, OTA_BINARY_NAME);
            t_httpUpdate_return ret = ESPhttpUpdate.update(binbuf);

            // I don't think this code actually is triggered unless HTTP fails.
            switch (ret)
            {
            case HTTP_UPDATE_FAILED:
                Serial.printf("[OTA] Download failed, Error: (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());

                indicate_error_download();
                for (;;)
                {
                    delay(200);
                }
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("[OTA] Your badge is running the latest firmware");
                // Latest version
                indicate_error_latest_version();
                delay(10000);
                ESP.restart();
                break;

            case HTTP_UPDATE_OK:
                Serial.println("[OTA] Your badge is updated!");
                ESP.restart();
                break;
            }
        }
        else
        {
            // Latest version
            indicate_error_latest_version();
            delay(10000);
            ESP.restart();
        }
        WiFi.disconnect();
    }
    else
    {
#ifdef DEBUG
        Serial.println("HTTP didn't throw a 200 :(");
        Serial.println(urlbuf);
        Serial.println(httpCode);
        WiFi.disconnect();
#endif

        Serial.println("[OTA] Download server error. Please try again later.");
        indicate_error_download();
        delay(10000);
        ESP.restart();
    }
}

#pragma endregion