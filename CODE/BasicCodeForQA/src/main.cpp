#include <Arduino.h>
#include <esp_wifi.h>

#include <TJpg_Decoder.h>
#include <FastLED.h>
#include <TFT_eSPI.h> 

#include "config.hpp"
#include "ota.hpp"
#include "bp_logo.hpp"

const int ota_button_pin = GPIO_NUM_0; // Boot button
const int LED_PINS[] = {10, 11, 12, 13, 14, 15};
const int NUM_NEOPIXELS = 3; 
const int NEO_BRIGHTNESS = 25; // Set BRIGHTNESS to about 1/5 (max = 255)
const int NEO_PIN = GPIO_NUM_18;
CRGB NEO_RGB_LEDS[NUM_NEOPIXELS];
TFT_eSPI tft = TFT_eSPI();

int NEO_MATRIX[][3] = {{1, 0 ,0}, {0, 1, 0},{0, 0, 1}};

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  tft.pushImage(x, y, w, h, bitmap);

  return 1;
}

void set_neo_backlight(const char * hexcolor, int activate_neos[NUM_NEOPIXELS]) {
    int r, g, b, r_t;
    sscanf(hexcolor, "%02x%02x%02x", &r, &g, &b);
    r_t = r; r = g; g = r_t; // Swapping red and blue due to a pinning issue?
    for (int i = 0; i < NUM_NEOPIXELS; i++){
        if (activate_neos[i])
            NEO_RGB_LEDS[i].setRGB(r, g, b);
    }
    FastLED.show();
}

void setup() {
    Serial.begin(115200);
    pinMode(ota_button_pin, INPUT_PULLUP);

    // Lower the power consumption without much impact to performance/usability
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
    setCpuFrequencyMhz(80);

    // Check for buttons held at boot
    if (!digitalRead(ota_button_pin)) { // If OTA update is requested, do this first.
        Serial.println("[Main] Starting OTA update");
        OTA::checkOTASync();
    }

    for (int i = 0; i < (sizeof(LED_PINS) / sizeof(LED_PINS[0])); i++) {
        pinMode(LED_PINS[i], OUTPUT);
    }

    FastLED.addLeds<WS2812B, NEO_PIN>(NEO_RGB_LEDS, NUM_NEOPIXELS); // Only try to realize the truth... there is no pin
    FastLED.setBrightness(255);
    NEO_RGB_LEDS[1].setRGB(0, 0, 0);
    NEO_RGB_LEDS[0].setRGB(0, 0, 0);
    NEO_RGB_LEDS[2].setRGB(0, 0, 0);
    FastLED.show();

    tft.init();
    tft.setSwapBytes(true);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(tft_output);

    tft.startWrite();
    TJpgDec.drawJpg(0, 0, BP_LOGO, sizeof(BP_LOGO));
    tft.endWrite();

    for (int i = 0; i < 6; i++) {
        digitalWrite(LED_PINS[i], 1);
        delay(100);
    }

    for (int i = 6; i > -1; i--) {
        digitalWrite(LED_PINS[i], 0);
        delay(100);
    }

}

void loop() {

    for (int i = 0; i < sizeof(NEO_MATRIX)/sizeof(NEO_MATRIX[0]); i++) {
        set_neo_backlight("FF0000", NEO_MATRIX[i]);
        delay(100);
    }

    for (int i = 0; i < sizeof(NEO_MATRIX)/sizeof(NEO_MATRIX[0]); i++) {
        set_neo_backlight("00FF00", NEO_MATRIX[i]);
        delay(100);
    }

    for (int i = 0; i < sizeof(NEO_MATRIX)/sizeof(NEO_MATRIX[0]); i++) {
        set_neo_backlight("0000FF", NEO_MATRIX[i]);
        delay(100);
    }

    // Check for OTA request
    if (!digitalRead(ota_button_pin)) {
        Serial.println("[Main] Starting OTA update");
        OTA::checkOTASync();
    }
}