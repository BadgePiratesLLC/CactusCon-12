#include "badgepirates/eeprom.hpp"
Preferences preferences;

void BMSEEPROM::init()
{
    // Start the preferences service
    preferences.begin("bms-eeprom", false);

    if (this->badge == "")
    {
        extern const char badge_txt[] asm("_binary_provisioning_data_badge_txt_start");
        String badge_data = String(badge_txt);
        preferences.putString("badge", badge_data);
    }

    if (this->event == "")
    {
        extern const char event_txt[] asm("_binary_provisioning_data_event_txt_start");
        String event_data = String(event_txt);
        preferences.putString("event", event_data);
    }

    if (this->version == "")
    {
        extern const char version_txt[] asm("_binary_provisioning_data_version_txt_start");
        String version_data = String(version_txt);
        preferences.putString("version", version_data);
    }

    // Attempt to get the values from EEPROM
    this->badge = preferences.getString("badge", "");
    this->event = preferences.getString("event", "");
    this->version = preferences.getString("version", "");
}
