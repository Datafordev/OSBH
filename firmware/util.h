/*
 * Open Source Beehives library
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include "stdint.h"


/* Constants */
#define ONE_HOUR_SECS (60*60)
#define ONE_DAY_MILLIS (24*60*60*1000)
#define TIME_EPOCH_START 1970
#define GMT_MIN_OFFSET -12
#define GMT_MAX_OFFSET 13

/* Forward declarations */
class SDClass;


namespace OSBH {

/* Utility functions */
    
bool init_wifi(uint16_t timeout_ms = 10000);

// return value may not be reliable indicator of success/failure
// in cases where time has already been synced previously and is
// being re-synced
bool sync_time(uint16_t timeout_ms = 1000);

// appends line to file on SD card. creates file if it doesn't exist.
bool write_to_sd(SDClass& sd, const char* line, const char* filename);

// clears buffer and returns false if the GMT offset is out of range
bool get_timestamp(char* buffer, const int len, float gmt_offset = 0.);

// appends either a comma or a line break, depending on whether last_entry is true.
// will overwrite the buffer contents with the delimiter if there isn't room to append
// it on the end.
void append_csv_delimiter(char* buffer, const int len, bool last_entry = false);

}


#endif
