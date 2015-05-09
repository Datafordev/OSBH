#include "application.h"
#include "OSBH.h"

// disable wifi on boot
SYSTEM_MODE(SEMI_AUTOMATIC);

// 1: debug printing, 0: no debug printing
#if 1 
    #define DEBUG(...) Serial.println(__VA_ARGS__);
#else
    #define DEBUG(...) do {} while (0)
#endif


void setup() 
{
    Serial.begin(9600);

    if (!init_wifi()) {
        DEBUG("could not establish wifi connection");
    }
}


void loop() 
{
    // timing variables
    static uint32_t next_sync;
    static uint32_t next_read;
    const uint32_t now = millis();

    // time synchronization
    if (now > next_sync) {
        DEBUG("syncing time...");
        sync_time();
        next_sync = now + ONE_DAY_MILLIS;
    }

    // read and report sensor data
    if (now > next_read) {
        DEBUG("reading...");

        next_read = now + READ_INTERVAL;
    }
}

