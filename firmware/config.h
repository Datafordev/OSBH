#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "application.h"


// 1: debug printing, 0: no debug printing
#define OSBH_DEBUG 1

#if OSBH_DEBUG
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__);
    #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__);
#else
    #define DEBUG_PRINT(...) do {} while (0)
    #define DEBUG_PRINTLN(...) do {} while (0)
#endif

/* Pin and sensor definitions */
#define ONE_WIRE_PIN   3
#define DHT_PIN1       4         // Pin for internal DHT sensor.
#define DHT_PIN2       5         // Pin for external DHT sensor.
#define DHT_TYPE       DHT22     // DHT 22 (AM2302)

/* Audio analysis parameters */
int MICROPHONE = 10;        // A0 on spark Core
int FFT_SIZE = 128;         //FFT Bucket Size (32,64,128,256 - higher means more frequency resolution)
int SAMPLEDELAY = 600;      //Delay for sampling in microseconds f = 1/t*10^6

/* Configuration variables */
#define IDEAL_READ_INTERVAL 5000 // may be adjusted upward to match sensors' min_delays

/* Output variables */
#define LOGFILE_NAME "osbh.csv"
#define DELIMITER ","
#define LINE_END "\n"
#define GMT_OFFSET -8


#endif