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
    tft.setCursor(0,0,4);
    tft.setTextColor(TFT_WHITE);
    tft.println ("Hello World!");
    //retcode = reader.drawBMP("/big_hacktar.bmp", tft, 0, 0);
    //https://forum.arduino.cc/t/st7789-draw-bmp-files-faster/685758/4
    #endif

    delay(1000);
    reader.printStatus(retcode);

    FastLED.addLeds<WS2812B, NEO_PIN>(neo_rgb_leds, NUM_NEOPIXELS); // Only try to realize the truth... there is no pin
    FastLED.setBrightness(10);
    TeleTrack::neo_rgb_leds[0] = CRGB::Black;
    TeleTrack::neo_rgb_leds[1] = CRGB::Black;
    TeleTrack::neo_rgb_leds[2] = CRGB::Black;
    FastLED.show();
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