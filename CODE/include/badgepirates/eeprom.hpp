
#include <Preferences.h>

class BMSEEPROM
{
public:
    void init();
    void loop();
    String getBadge() { return badge; }
    String getEvent() { return event; }
    String getVersion() { return version; }

private:
    String badge;
    String event;
    String version;
};