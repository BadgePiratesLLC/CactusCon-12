#include "teletrack.hpp"
CRGB TeleTrack::neo_rgb_leds[NUM_NEOPIXELS];
#ifdef BEES3
TFT_eSPI tft = TFT_eSPI();
#endif
SPIFFS_ImageReader reader;
RotaryEncoder TeleTrack::left_rotary_encoder(LEFT_ENCODER_A, LEFT_ENCODER_B, LEFT_ENCODER_SW);
/**
 * @brief Initalizes pin modes for various indicators.
*/
void TeleTrack::setup() {
    ImageReturnCode retcode;
    pinMode(UPLINK_LED, OUTPUT);
    TeleTrack::set_uplink_light(false);

    left_rotary_encoder.setEncoderType(EncoderType::HAS_PULLUP);
    left_rotary_encoder.setBoundaries( -1, 1, false );
    left_rotary_encoder.onTurned(&TeleTrack::left_rotary_encoder_callback);
    left_rotary_encoder.begin();


    if(!SPIFFS.begin()) 
        Serial.println("SPIFFS initialisation failed!");

    #ifdef BEES3
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(40,104,4);
    tft.setTextColor(TFT_WHITE);
    tft.println ("Cactus Con 12");
    tft.setCursor(45,125,4);
    tft.setTextColor(TFT_RED);
    tft.println ("test code v0.1");
    //retcode = reader.drawBMP("/big_hacktar.bmp", tft, 0, 0);
    //https://forum.arduino.cc/t/st7789-draw-bmp-files-faster/685758/4
    #endif

    delay(1000);
    reader.printStatus(retcode);
 
            FastLED.addLeds<WS2812B, NEO_PIN>(neo_rgb_leds, NUM_NEOPIXELS); // Only try to realize the truth... there is no pin
            FastLED.setBrightness(100);

            CRGB red = CRGB(255, 0, 0);
            CRGB green = CRGB(0, 255, 0);
            CRGB blue = CRGB(0, 0, 255);
            CRGB purple = CRGB(128, 0, 128); // Purple color
// ---------- ADDED BELOW by FG for Testing the LEDs if it is in Test Mode -----------
        // Set LED colors in a pattern
        if (TEST_MODE) {
    Serial.println("Testing RGB LEDS");

        for (int i = 0; i < NUM_NEOPIXELS; i++) {
            neo_rgb_leds[i] = red;
            FastLED.show();
            delay(250);
            neo_rgb_leds[i] = green;
            FastLED.show();
            delay(250);
            neo_rgb_leds[i] = blue;
            FastLED.show();
            delay(250);
            neo_rgb_leds[i] = purple;
            FastLED.show();
            delay(250);
        }

        // Clear the LEDs
        for (int i = 0; i < NUM_NEOPIXELS; i++) {
            neo_rgb_leds[i] = CRGB::Black; // Turn off the LED
        }
            FastLED.show();
        Serial.println("Static LEDS");
            // Testing the LEDs
        for (int i = 0; i < 6; i++) {
            pinMode(ledPins[i], OUTPUT); // Set LED pins as output
        }
        // Blink the 6 static LEDs on
        for (int i = 0; i < 6; i++) {
            digitalWrite(ledPins[i], HIGH); // Turn the LED on
        }
        delay(1000); 

        // Turn off all LEDs
        for (int i = 0; i < 6; i++) {
            digitalWrite(ledPins[i], LOW); // Turn the LED off
        }
        delay(1000); 
            
    // Additional test-specific setup steps can go here
  } else {
  }
}

void TeleTrack::left_rotary_encoder_callback(long value) {
    Serial.printf( "Value: %i\n", value );
}

/**
 * @brief Enables/disables the uplink light.
 * @param[in] on_or_off True for on, False for off.
*/
void TeleTrack::set_uplink_light(bool on_or_off) {
    digitalWrite(UPLINK_LED, on_or_off);
}

/**
 * @brief Set a score on the display
 * @param[in] on_or_off True for on, False for off.
*/
void TeleTrack::set_score(int score) {
    // 200, 95 - furthest right on screen for text size 6
    // 5, 95 - furthest left on screen for text size 6
    // text size each digit is 25 pixels
    char buf[255];
    int16_t x1, y1;
    uint16_t w, h;
    int x = 123;
    int y = 95;
    snprintf(buf, 255, "%d", score);
#ifdef BEES3
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(6);
    if (score > 0)
        tft.setTextColor(TFT_WHITE);
    else
        tft.setTextColor(TFT_RED);
    tft.setTextDatum(MC_DATUM); tft.drawString(buf, tft.width() / 2, tft.height() / 2);
#endif
}