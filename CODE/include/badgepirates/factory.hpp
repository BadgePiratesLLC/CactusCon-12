#pragma once
#define AUTOMATIC_UPDATES           // Comment out to disable, Does not require boot to be pressed to initialize an update.
#define AUTOMATIC_UPDATE_DELAY 5000 // Wait X second before beginning the update process

#include <Arduino.h>
#include <BfButton.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "FastLedInclude.hpp"
#include "config.hpp"
#include "ota.hpp"

// Add all "simple" leds here, you will need to increase the array size accordingly.
const int all_leds[9] = {10,11,12,13,14,15,3,4,5};

// Buttons
const int down_btn_pin = GPIO_NUM_38;
const int up_btn_pin = GPIO_NUM_37;
const int center_btn_pin = GPIO_NUM_36;
const int right_btn_pin = GPIO_NUM_35;
const int left_btn_pin = GPIO_NUM_46;

class Factory
{
private:
    // Any additional code, for running screens, inputs, etc.
    Adafruit_SSD1306 *display;

    void test_oled(gpio_num_t sda, gpio_num_t scl);
    void button_setup();
    void leds_setup();
    void leds_off();
    void leds_on();
    void leds_test();

public:
    void setup();
    void loop();
    void test_ws2812();
};