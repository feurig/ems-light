/*------------------------------------------------------------------
 * Common defines for EMS hardware
 */

#ifndef EMS_COMMON_h
#define EMS_COMMON_h

#include <Arduino.h>

#define GIT_VERSION "xxxxxxxxxxxxxxxxx"
#define MAX_PSN_LENGTH 33
//
// reccommend major and minor rev plus build date.
//
#ifndef FIRMWAREREV
#define FIRMWAREREV "EMS1"
#endif

#define SAFETY_LAMP_PIN   13
//
// Define E_STOP_PIN to enable estop switch
//
//#define E_STOP_PIN x
#ifndef E_STOP_PULLED
#define E_STOP_PULLED 1
#endif

//
// Define LIGHT_CURTAIN pins to enable light curtain
//
//#define LIGHT_CURTAIN_OSDD_PIN    5
//#define LIGHT_CURTAIN_START_PIN   6
#define LIGHT_CURTAIN_START LOW
#define LIGHT_CURTAIN_STOP HIGH
#define LIGHT_CURTAIN_OK LOW
#define LIGHT_CURTAIN_NOT_CLEAR HIGH

//
// Define K1_PIN if konactor is connected to machine
//
//#define K1_PIN x

// 
#define POST_TIMESTAMP_DELIMETER ','

// analog write resolution (new for m0s)
#define ANALOG_WRITE_RESOLUTION 8

// tzoffset from localtime (PDT) to GMT (7 hrs) in seconds
#define LOCAL_TZ_OFFSET ( 7L * 3600L )

// non volitile memory addresses go here...... 
#define CLEAN_SHUTDOWN_EEPROM_ADDR ((uint8_t *)0)
#define CLEAN_SHUTDOWN_CODE 0xAA   //unlikely to randomly be in there
#define LOG_LEVEL_EEPROM_ADDR ((uint8_t*) 2)
#define DEBUG_LEVEL_EEPROM_ADDR ((uint8_t*) 3)



#define MONITOR_UART Serial
#define MONITOR_BAUD 9600

#define FAKE_A_FLOAT(_f) int((_f)), int(abs((_f) - int(_f))*100)

#endif
