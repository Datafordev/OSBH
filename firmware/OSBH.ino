#include "config.h"
#include "util.h"
#include "Sensor_Array.h"

// disable wifi on boot
SYSTEM_MODE(SEMI_AUTOMATIC);

// global sensor array
OSBH::Sensor_Array sensors(ONE_WIRE_PIN, DHT_PIN1, DHT_PIN2, DHT_TYPE);

// actual read interval - may be modified from #defined READ_INTERVAL
// depending on the min_delay values of the sensors.
uint32_t read_interval = IDEAL_READ_INTERVAL;


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

    // make sure our read interval respects our minimum sensor delays
    int32_t min_delay = sensors.min_delay();
    if (min_delay > 0 && min_delay > read_interval) {
        read_interval = min_delay;
    }

    DEBUG_PRINT("setup complete");
}


void loop() 
{
    // timing variables
    static uint32_t next_sync;
    static uint32_t next_read;
    const uint32_t now = millis();

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
        Adafruit_Sensor** array = sensors.array();
        for (uint8_t i = 0; i < OSBH::Sensor_Array::MAX_SENSOR_CNT; ++i) {
            Adafruit_Sensor* s = array[i];
            if (!s) continue;

            s->getSensor(&sensor);
            s->getEvent(&event);
            DEBUG_PRINT(String(sensor.name) + " " + String(sensor.sensor_id) + ": " + String(event.data[0]));
        }

        next_read = now + read_interval;
    }
}

