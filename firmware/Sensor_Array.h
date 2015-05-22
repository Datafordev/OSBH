#ifndef __SENSOR_ARRAY_H__
#define __SENSOR_ARRAY_H__

#include "DSX_U.h"
#include "DHT_U.h"
#include "Adafruit_Sensor.h"


namespace OSBH {

/* Sensor array wrapper */

class Sensor_Array
{
public:
    // maximum number of sensors, assuming two DHT sensors
    // TODO: modify this class to accept a variable number of DHT sensors
    static const uint8_t MAX_SENSOR_CNT = DSX_Unified::MAX_SENSORS + 4;

    // Construct individual sensor objects, but don't yet initialize the array
    Sensor_Array(uint8_t one_wire_pin, uint8_t dht_pin1, uint8_t dht_pin2, uint8_t dht_type);

    // Initialize the array
    void begin();

    // Array of sensor pointers. Call begin() before accessing this.
    Adafruit_Sensor** array() { return _sensors; }

    // The longest min_delay for all the sensors in the array.
    // Call begin() before accessing this.
    int32_t min_delay() const { return _min_delay; }

private:
    DSX_Unified _dsx;
    DHT_Unified _dht;
    DHT_Unified _dht2;
    Adafruit_Sensor* _sensors[8];
    int32_t _min_delay;

    void setMinDelay();
};

}


#endif