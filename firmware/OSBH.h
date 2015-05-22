/*
 * Open Source Beehives library
 */

#ifndef __OSBH_H__
#define __OSBH_H__

#include "application.h"

 // 1: debug printing, 0: no debug printing
#define DEBUG 1

#if DEBUG
    #define DEBUG_PRINT(...) Serial.println(__VA_ARGS__);
#else
    #define DEBUG_PRINT(...) do {} while (0)
#endif


/* Pin definitions */
#define ONE_WIRE_PIN 3


/* Configuration variables */
#define READ_INTERVAL 1000


/* Constants */
#define ONE_DAY_MILLIS (24*60*60*1000)
#define TIME_EPOCH_START 1970


/* Utility functions */
// FIXME: spark core build system throws 'undefined reference' errors when
// the function definitions are moved to the .cpp file, despite the cpp file
// being listed in spark.include

bool init_wifi(uint16_t timeout_ms = 6000)
{
    WiFi.on();
    Spark.connect();
    uint32_t timeout = millis() + timeout_ms;
    while (!Spark.connected() && millis() < timeout) {
        Spark.process();
        delay(100);
    }
    return Spark.connected();
}

void sync_time(uint16_t timeout_ms = 1000)
{
    if (Spark.connected()) {
        Spark.syncTime();

        // syncTime is non-blocking, so wait for a valid time to come back
        // from the server before proceeding
        uint32_t timeout = millis() + timeout_ms;
        while (Time.year() <= TIME_EPOCH_START && millis() < timeout) {
            Spark.process();
            delay(100);
        }
    }
}


#endif
