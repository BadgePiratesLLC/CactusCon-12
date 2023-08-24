#include "badgepirates/factory.hpp"

void Factory::test_ws2812()
{
    // Insert anything that needs to be tested here.
    FastLED.setBrightness(45);
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::White;
        FastLED.show();
        delay(30);
    }
    delay(100);
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Black;
        FastLED.show();
        delay(30);
    }
}

void Factory::test_oled(gpio_num_t sda, gpio_num_t scl)
{
    Wire.begin(sda, scl);
    display = new Adafruit_SSD1306(128, 64, &Wire, -1); // Using a GME12864-17 module which is a SSD1306
    if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C))    // OLED will not always be plugged in.
    {                                                   ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
    }
    else
    {
        // Show initial display buffer contents on the screen --
        // the library initializes this with an Adafruit splash screen.
        display->display();
    }
    Wire.end();
}

void Factory::leds_setup()
{
    for (int led : all_leds)
    {
        pinMode(led, OUTPUT);
    }
}

void Factory::leds_on()
{
    for (int led : all_leds)
    {
        digitalWrite(led, HIGH);
        delay(50); // Gives a slight animation
    }
}

void Factory::leds_off()
{
    for (int led : all_leds)
    {
        digitalWrite(led, LOW);
        delay(50); // Gives a slight animation
    }
}

// Simple LEDs only!
void Factory::leds_test()
{
    leds_on();
    leds_off();
    leds_on();
}

void Factory::button_setup()
{
    // Init
    pinMode(down_btn_pin, INPUT_PULLUP);
    pinMode(up_btn_pin, INPUT_PULLUP);
    pinMode(center_btn_pin, INPUT_PULLUP);
    pinMode(right_btn_pin, INPUT_PULLUP);
    pinMode(left_btn_pin, INPUT_PULLUP);
}

void Factory::setup()
{
#if VERSION > 0
    Serial.println("[FACTORY] WARNING: Factory code is enabled on production versions");
#endif

    test_ws2812();
    // Primary OLED
    test_oled(GPIO_NUM_4, GPIO_NUM_3);
    // Accessory OLED
    test_oled(GPIO_NUM_6, GPIO_NUM_7);
    button_setup();
    leds_setup();
    leds_test();

#ifdef AUTOMATIC_UPDATES
    delay(AUTOMATIC_UPDATE_DELAY);
    leds_off();
    // OTA::checkOTASync();
#else
    pinMode(GPIO_NUM_0, INPUT_PULLUP);
    Serial.println("Hold BOOT to initialize an update.");
#endif
}

void Factory::loop()
{
#ifndef AUTOMATIC_UPDATES
    auto boot_buttonState = digitalRead(GPIO_NUM_0);
    auto down_buttonState = digitalRead(down_btn_pin);
    auto up_buttonState = digitalRead(up_btn_pin);
    auto center_buttonState = digitalRead(center_btn_pin);
    auto right_buttonState = digitalRead(right_btn_pin);
    auto left_buttonState = digitalRead(left_btn_pin);

    if (boot_buttonState == LOW)
    { // Pin state ref: https://www.esp32.com/viewtopic.php?t=869, Change if board is different.
        leds_off();
        // OTA::checkOTASync();
    }

    if (down_buttonState == LOW)
    {
        leds[0] = CRGB::Blue;
        FastLED.show();
    }
    else if (up_buttonState == LOW)
    {
        leds[0] = CRGB::Green;
        FastLED.show();
    }
    else if (center_buttonState == LOW)
    {
        leds[0] = CRGB::White;
        FastLED.show();
    }
    else if (right_buttonState == LOW)
    {
        leds[0] = CRGB::Purple;
        FastLED.show();
    }
    else if (left_buttonState == LOW)
    {
        leds[0] = CRGB::Red;
        FastLED.show();
    }
    else
    {
        leds[0] = CRGB::Black;
        FastLED.show();
    }

    delay(20);
#else
    delay(100);
#endif
}