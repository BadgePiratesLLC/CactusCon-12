#include <Arduino.h>

#include "badgepirates/eeprom.hpp"
#include "badgepirates/ble.hpp"
#include "badgepirates/factory.hpp"
#include "badgepirates/FastLedInclude.hpp"
#include "config.hpp"

struct CRGB leds[NUM_LEDS];

#ifdef BLE_MODE
MyBLEServer ble;
#endif

#ifdef FACTORY_MODE
Factory factory;
#endif

BMSEEPROM bmsData;

void setup()
{
  Serial.begin(115200);
  bmsData.init();
  Serial.printf("Badge: %s \r\n Event: %s \r\n Version: %s \r\n", bmsData.getBadge().c_str(), bmsData.getEvent().c_str(), bmsData.getVersion().c_str());
  // FastLED must be set up on the main class
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
#ifdef BLE_MODE
  ble.init();
#endif

#ifdef FACTORY_MODE
  factory.setup();
#endif
}

void loop()
{
#ifdef BLE_MODE
  ble.loop();
#endif
#ifdef FACTORY_MODE
  factory.loop();
#endif
}