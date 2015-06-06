#include "util.h"
#include "application.h"
#include "sd-card-library.h"


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


bool OSBH::sync_time(uint16_t timeout_ms)
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
        return millis() < timeout;
    }
    return false;
}

bool OSBH::write_to_sd(SDClass& sd, const char* line, const char* filename) 
{
    if (File myFile = sd.open(filename, FILE_WRITE)) {
        myFile.print(line);
        myFile.close();
        return true;
    }
    return false;
}

bool OSBH::get_timestamp(char* buffer, const int len, float gmt_offset)
{
    // return empty string if the GMT offset doesn't make sense
    if (gmt_offset < GMT_MIN_OFFSET || gmt_offset > GMT_MAX_OFFSET) {
        buffer[0] = '\0';
        return false;
    }

    // get pointer to unix time string
    time_t t = Time.now();
    struct tm *calendar_time;
    t += gmt_offset * ONE_HOUR_SECS;
    calendar_time = localtime(&t);
    char* time_str = asctime(calendar_time);

    // copy into buffer
    // (we need to make a copy because the pointed-to string will change on
    // subsequent calls to asctime)
    int timestamp_len = min(len, strlen(time_str));
    strncpy(buffer, time_str, timestamp_len);

    // drop trailing line break from time_str
    buffer[timestamp_len - 1] = '\0';

    return true;
}

void OSBH::append_csv_delimiter(char* buffer, const int len, bool last_entry)
{
    // delimiter is comma for non-final entries, line break for final ones
    static const char final_suffix[3] = "\r\n";
    static const char non_final_suffix[2] = ",";
    const char* suffix = last_entry ? final_suffix : non_final_suffix;

    // append delimiter to end of string in io buffer. This may overwrite
    // buffer contents if there isn't enough space to add it at the end.
    int suffix_len = strlen(suffix) + 1;
    int string_len = min(strlen(buffer), len - suffix_len);
    strncpy(buffer + string_len, suffix, suffix_len);
}