#include "config.h"
#include "util.h"
#include "Sensor_Array.h"
#include "sd-card-library.h"
#include "audio.h"

using namespace OSBH;

// disable wifi on boot
SYSTEM_MODE(SEMI_AUTOMATIC);

// global sensor array
Sensor_Array sensors(ONE_WIRE_PIN, DHT_PIN1, DHT_PIN2, DHT_TYPE);

// whether there's an SD card present in the socket
enum SD_State { SD_REMOVED, SD_READY, SD_UNPREPARED };
SD_State sd_state = SD_REMOVED;

// buffer for reading/writing strings. Using the spark/wiring String class
// would be more convenient, but risks memory fragmentation.
static const int IO_BUFFER_LEN = 36;
char io_buffer[IO_BUFFER_LEN];

/**************************************************************************/
/*
        Utility functions
 */
/**************************************************************************/

// writes the contents of io_buffer to all the currently active output destinations.
// last_entry indicates whether the contents of the buffer fall at the end of a
// line of sensor data.
void write(bool last_entry = false)
{
    // double-check that buffer is null-terminated
    io_buffer[IO_BUFFER_LEN-1] = '\0';

    // prep for writing as CSV
    append_suffix(io_buffer, IO_BUFFER_LEN, (last_entry ? LINE_END : DELIMITER));

    // save data to SD card, if it's ready
    if (sd_state == SD_READY && !write_to_sd(SD, io_buffer, LOGFILE_NAME)) {
        DEBUG_PRINTLN("could not write to SD card");
        sd_state = SD_UNPREPARED; // attempt to re-initialize later
    } 

    // print to debug output
    DEBUG_PRINT(io_buffer);
}

// interrupt handler for SD card
void update_sd_state()
{
    if (!digitalRead(SD_CD_PIN)) {
        // card is absent
        sd_state = SD_REMOVED;
    } else if (sd_state == SD_REMOVED) {
        // card is present, but was previously removed, so requires initialization
        sd_state = SD_UNPREPARED;
    }
}

/**************************************************************************/
/*
        Function:  setup
 */
/**************************************************************************/
void setup() 
{
    Serial.begin(9600);
    DEBUG_PRINTLN("starting setup...");

    // attempt to connect to wifi
    if (!init_wifi()) {
        DEBUG_PRINTLN("could not establish wifi connection");
    }

    // initialize sensors
    sensors.begin();

    // reserve memory for FFT (audio analysis)
    FFTinit();

    // attach interrupt on SD card-detect pin to catch inserts/removes
    pinMode(SD_CD_PIN, INPUT_PULLUP);
    attachInterrupt(SD_CD_PIN, update_sd_state, CHANGE);
    update_sd_state(); // set initial state

    // write header for timestamp column
    strncpy(io_buffer, "Timestamp", IO_BUFFER_LEN);
    write();

    // write sensor names/IDs as CSV headers
    for (uint8_t i = 0; i < sensors.count(); ++i) {
        sensors.getSensorString(i, io_buffer, IO_BUFFER_LEN);
        write(i == sensors.count() - 1);
    }

    DEBUG_PRINTLN("setup complete");
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
        DEBUG_PRINTLN("syncing time...");
        sync_time();
        next_sync = now + ONE_DAY_MILLIS;
    }

    // initialize SD card if necessary
    if (sd_state == SD_UNPREPARED) {
        SD.begin();
        sd_state = SD_READY;
    }

    // read and report sensor data
    if (now > next_read) {
        DEBUG_PRINTLN("reading...");

        // write timestamp
        get_timestamp(io_buffer, IO_BUFFER_LEN, GMT_OFFSET);
        write();

        // get sensor readings
        for (uint8_t i = 0; i < sensors.count(); ++i) {
            sensors.getEventString(i, io_buffer, IO_BUFFER_LEN);
            write(i == sensors.count() - 1);
        }

        //perform audio analysis
        updateFFT();
        printfrequencies();

        next_read = now + read_interval;
    }
}

