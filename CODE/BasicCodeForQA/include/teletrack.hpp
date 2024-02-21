#pragma once
#include "Arduino.h"
#include "Wire.h"
#include <FastLED.h>
#include <SPIFFS_ImageReader.h>
#include <ESP32RotaryEncoder.h>
#include <TFT_eSPI.h>

#ifdef BEES3
const int NUM_NEOPIXELS = 3; 
const int NEO_BRIGHTNESS = 25; // Set BRIGHTNESS to about 1/5 (max = 255)
const int NEO_PIN = 18; 
const int ledPins[] = {10, 11, 12, 13, 14, 15};
const int UPLINK_LED = GPIO_NUM_12;
const int LEFT_ENCODER_A = GPIO_NUM_4;
const int LEFT_ENCODER_B = GPIO_NUM_5;
const int LEFT_ENCODER_SW = GPIO_NUM_3;
const int RIGHT_ENCODER_A = GPIO_NUM_16;
const int RIGHT_ENCODER_B = GPIO_NUM_17;
const int RIGHT_ENCODER_SW = GPIO_NUM_10;
#else
const int NUM_NEOPIXELS = 1; 
const int NEO_BRIGHTNESS = 25; // Set BRIGHTNESS to about 1/5 (max = 255)
const int NEO_PIN = 17; 
const int UPLINK_LED = GPIO_NUM_10;
const int DISPLAY_SDA = GPIO_NUM_6;
const int DISPLAY_SCL = GPIO_NUM_7;
#endif

/**
 * @brief High-level interface for working with the on-board indicators. Call TeleTrack::setup() before use.
*/
class TeleTrack {
    private:
        static CRGB neo_rgb_leds[NUM_NEOPIXELS];
        static RotaryEncoder left_rotary_encoder;
        static volatile bool left_turned_right_flag;
        static volatile bool left_turned_left_flag;
    public:
        static void setup();
        static void set_uplink_light(bool on_or_off);
        static void set_score(int score);
        static void left_rotary_encoder_callback(long value);
};