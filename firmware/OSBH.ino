#include "OSBH.h"
#include "DSX_U.h"
#include "DHT_U.h"

// disable wifi on boot
SYSTEM_MODE(SEMI_AUTOMATIC);

// global sensor variables
// TODO: wrap this in something that just exposes a single Adafruit_Sensor* array
DSX_Unified dsx(ONE_WIRE_PIN);
DHT_Unified dht(DHT_PIN, DHT_TYPE, 6, 1, 2);
DHT_Unified dht2(DHTB_PIN, DHT_TYPE, 6, 3, 4);
#define SENSOR_CNT (DSX_Unified::MAX_SENSORS + DHT_CNT*2)
Adafruit_Sensor* _sensors[SENSOR_CNT] = {nullptr};

// actual read interval - may be modified from #defined READ_INTERVAL
// depending on the min_delay values of the sensors.
uint32_t read_interval = IDEAL_READ_INTERVAL;


void setup() 
{
    Serial.begin(9600);

    if (!init_wifi()) {
        DEBUG_PRINT("could not establish wifi connection");
    }

    // initialize DHT sensors
    dht.begin();
    dht2.begin();

    // initialize DSX sensors
    // NOTE: currently these do not have unique names. Giving them names that reference
    // their positions in the hive is potentially problematic because in the case of a
    // sensor failure we won't know which one is missing so won't be able to accurately
    // ID the others on the bus.
    dsx.prepare(5); // DHT sensors are IDs 1-4

    // copy sensor pointers into single array
    _sensors[0] = dht.temperature();
    _sensors[1] = dht.humidity();
    _sensors[2] = dht2.temperature();
    _sensors[3] = dht2.humidity();
    memcpy(_sensors+4, dsx.sensorArray(), dsx.children_cnt() * sizeof(_sensors[0]));

    // ensure the read interval isn't faster than the min read interval of the
    // sensors
    sensor_t sensor;
    for (Adafruit_Sensor** ptr = _sensors; ptr < _sensors + SENSOR_CNT; ++ptr) {
        Adafruit_Sensor* s = *ptr;
        if (s) {
            s->getSensor(&sensor);
            int32_t min_read = sensor.min_delay / 1000; // usecs -> msecs
            if (min_read > 0 && read_interval < min_read) {
                read_interval = min_read;
            }
        }
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
        sync_time();
        next_sync = now + ONE_DAY_MILLIS;
    }

    // read and report sensor data
    if (now > next_read) {
        DEBUG_PRINT("reading...");

        // get sensor names and readings
        sensor_t sensor;
        sensors_event_t event;
        for (Adafruit_Sensor** ptr = _sensors; ptr < _sensors + SENSOR_CNT; ++ptr) {
            Adafruit_Sensor* s = *ptr;
            if (s) {
                s->getSensor(&sensor);
                s->getEvent(&event);
                DEBUG_PRINT(String(sensor.name) + " " + String(sensor.sensor_id) + ": " + String(event.data[0]));
            }
        }

        next_read = now + read_interval;
    }
}

