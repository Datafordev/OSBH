#include "OSBH.h"
#include "DSX_U.h"

// disable wifi on boot
SYSTEM_MODE(SEMI_AUTOMATIC);

// global sensor variables
DSX_Unified dsx(ONE_WIRE_PIN);


void setup() 
{
    Serial.begin(9600);

    if (!init_wifi()) {
        DEBUG_PRINT("could not establish wifi connection");
    }

    // initialize DSX sensors
    // NOTE: currently these do not have unique names. Giving them names that reference
    // their positions in the hive is potentially problematic because in the case of a
    // sensor failure we won't know which one is missing so won't be able to accurately
    // ID the others on the bus.
    dsx.prepare(0);

    // print the DSX sensor info
    sensor_t sensor;
    Adafruit_Sensor* s = *dsx.sensorArray();
    for (Adafruit_Sensor* child = s; child < s + dsx.children_cnt(); ++child) {
        child->getSensor(&sensor);
        DEBUG_PRINT(sensor.name);
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
        DEBUG_PRINT("syncing time...");
        sync_time();
        next_sync = now + ONE_DAY_MILLIS;
    }

    // read and report sensor data
    if (now > next_read) {
        DEBUG_PRINT("reading...");

        // get DSX readings
        sensors_event_t event;
        Adafruit_Sensor* s = *dsx.sensorArray();
        for (Adafruit_Sensor* child = s; child < s + dsx.children_cnt(); ++child) {
            // note for future error-handling - the DS18B20 returns 0. if the connection
            // has been lost
            child->getEvent(&event);
            DEBUG_PRINT(event.data[0]);
        }

        next_read = now + READ_INTERVAL;
    }
}

