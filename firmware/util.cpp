#include "util.h"


bool OSBH::init_wifi(uint16_t timeout_ms)
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


void OSBH::sync_time(uint16_t timeout_ms)
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