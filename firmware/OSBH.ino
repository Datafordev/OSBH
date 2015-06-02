#include "config.h"
#include "util.h"
#include "Sensor_Array.h"
#include "sd-card-library.h"

// disable wifi on boot
SYSTEM_MODE(SEMI_AUTOMATIC);

// global sensor array
OSBH::Sensor_Array sensors(ONE_WIRE_PIN, DHT_PIN1, DHT_PIN2, DHT_TYPE);

// whether an SD card is attached
bool SD_attached = true;

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

    // initialize SD card, if applicable
    if(SD_attached && !SD.begin()) {
        DEBUG_PRINT("could not initialize SD card");
        SD_attached = false;
    }

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
        String out = OSBH::timestamp() + ",";
        for (uint8_t i = 0; i < sensors.count(); ++i) {
            sensors.getSensor(i, &sensor);
            sensors.getEvent(i, &event);
            out += String(sensor.name) + "," + String(sensor.sensor_id) + "," + String(event.data[0]) + ",";
        }
        DEBUG_PRINT(out);

        // save data to SD card, if it's attached
        if (SD_attached && !OSBH::write_line_to_sd(SD, out, LOGFILE_NAME)) {
            DEBUG_PRINT("failed to write to SD card");
        }

        next_read = now + read_interval;
    }
}

