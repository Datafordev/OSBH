/*
 * Open Source Beehives library
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include "application.h"


/* Constants */
#define ONE_DAY_MILLIS (24*60*60*1000)
#define TIME_EPOCH_START 1970

/* Forward declarations */
class SDClass;


namespace OSBH {

/* Utility functions */
bool init_wifi(uint16_t timeout_ms = 10000);
void sync_time(uint16_t timeout_ms = 1000);
bool write_line_to_sd(SDClass& sd, const String& line, const char* filename);
String timestamp();

}


#endif
