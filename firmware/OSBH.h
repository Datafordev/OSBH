/*
 * Open Source Beehives library
 */

 #include "application.h"

/* Configuration variables */
#define READ_INTERVAL 1000


/* Constants */
#define ONE_DAY_MILLIS (24*60*60*1000)
#define TIME_EPOCH_START 1970


/* Utility functions */
bool init_wifi()
{
    WiFi.on();
    Spark.connect();
    return Spark.connected();
}

void sync_time()
{
    if (Spark.connected()) {
        Spark.syncTime();

        // syncTime is non-blocking, so wait for a valid time to come back
        // from the server before proceeding
        uint32_t timeout = millis() + 1000;
        while (Time.year() <= TIME_EPOCH_START && millis() < timeout) {
            Spark.process();
            delay(100);
        }
    }
}
