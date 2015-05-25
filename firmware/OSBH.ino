#include "config.h"
#include "util.h"
#include "Sensor_Array.h"

// disable wifi on boot
SYSTEM_MODE(SEMI_AUTOMATIC);

// global sensor array
OSBH::Sensor_Array sensors(ONE_WIRE_PIN, DHT_PIN1, DHT_PIN2, DHT_TYPE);

/**************************************************************************/
/*
        Function:  setup
 */
/**************************************************************************/
void setup() 
{
    Serial.begin(9600);
    DEBUG_PRINT("starting setup...");

    // attempt to connect to wifi
    if (!OSBH::init_wifi()) {
        DEBUG_PRINT("could not establish wifi connection");
    }

    // initialize sensors
    sensors.begin();

    DEBUG_PRINT("setup complete");
}

/**************************************************************************/
/*
        Function:  loop
 */
/**************************************************************************/
void loop() 
{
    // timing variables
    static uint32_t next_sync;
    static uint32_t next_read;
    const uint32_t now = millis();

    // make sure our read interval respects our minimum sensor delays
    static const uint32_t read_interval = max(IDEAL_READ_INTERVAL, sensors.minDelay());

    // time synchronization
    if (now > next_sync) {
        DEBUG_PRINT("syncing time...");
        OSBH::sync_time();
        next_sync = now + ONE_DAY_MILLIS;
    }

    // read and report sensor data
    if (now > next_read) {
        DEBUG_PRINT("reading...");

        // get sensor names and readings
        sensor_t sensor;
        sensors_event_t event;
        for (uint8_t i = 0; i < sensors.count(); ++i) {
            sensors.getSensor(i, &sensor);
            sensors.getEvent(i, &event);
            DEBUG_PRINT(String(sensor.name) + " " + String(sensor.sensor_id) + ": " + String(event.data[0]));
        }

        next_read = now + read_interval;
    }
}

