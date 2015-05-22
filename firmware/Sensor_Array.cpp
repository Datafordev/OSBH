#include "Sensor_Array.h"

using namespace OSBH;

Sensor_Array::Sensor_Array(uint8_t one_wire_pin, uint8_t dht_pin1, uint8_t dht_pin2, uint8_t dht_type) 
    : _dsx(one_wire_pin),
    _dht(dht_pin1, dht_type, 6, 1, 2),
    _dht2(dht_pin2, dht_type, 6, 3, 4),
    _sensors({nullptr}),
    _min_delay(0)
    {}


void Sensor_Array::begin()
{
    // initialize DHT sensors
    _dht.begin();
    _dht2.begin();

    // initialize DSX sensors
    // NOTE: currently these do not have unique names. Giving them names that reference
    // their positions in the hive is potentially problematic because in the case of a
    // sensor failure we won't know which one is missing so won't be able to accurately
    // ID the others on the bus.
    _dsx.prepare(5); // DHT sensors are IDs 1-4

    // copy sensor pointers into single array
    _sensors[0] = _dht.temperature();
    _sensors[1] = _dht.humidity();
    _sensors[2] = _dht2.temperature();
    _sensors[3] = _dht2.humidity();
    memcpy(_sensors+4, _dsx.sensorArray(), _dsx.children_cnt() * sizeof(_sensors[0]));

    setMinDelay();
}


void Sensor_Array::setMinDelay()
{
    sensor_t sensor;
    for (uint8_t i = 0; i < Sensor_Array::MAX_SENSOR_CNT; ++i) {
        Adafruit_Sensor* s = _sensors[i];
        if (!s) continue;

        s->getSensor(&sensor);
        int32_t d = sensor.min_delay / 1000; // usecs -> msecs
        if (d > _min_delay) {
            _min_delay = d;
        }
    }
}
