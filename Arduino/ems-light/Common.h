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

#define LAMP_PIN   13
#define TS1_CS_PIN 19
#define TS2_CS_PIN 18
#define PS1_APIN    1

#define POWER_PIN  17

#define E_STOP_PIN 16
#define E_STOP_PULLED HIGH
#define E_STOP_OK HIGH

#define OSDD_PIN    5
#define LIGHT_CURTAIN_OK LOW
#define CURTAIN_NOT_CLEAR HIGH

#define LAMP_PIN   13
#define HTR1_PIN   11
#define HTR2_PIN   12
#define PUMP_PIN   10
#define START_PIN   6
#define OK_TO_START LOW
#define SOLENOID_PIN 9
#define RELAY_6_PIN 14

#define K1_PIN     14
#define KONTACTOR_OFF LOW
#define KONTACTOR_ON  HIGH

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


#define DEFAULT_HEATER_SETPOINT 260.00
#define DEFAULT_TIMER_SETPOINT 150


#define MONITOR_UART Serial
#define MONITOR_BAUD 9600
//#define LOGGER_UART Serial1
//#define LOGGER_BAUD 9600

#define FAKE_A_FLOAT(_f) int((_f)), int(abs((_f) - int(_f))*100)

