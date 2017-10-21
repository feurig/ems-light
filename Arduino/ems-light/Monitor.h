/*---------------------------------------------------------------------Monitor.h
   Author: Joseph Wayne Dumoulin, Donald Delmar Davis, Suspect Devices

   Liscence: "Simplified BSD License"

   Copyright (c) 2016, Donald Delmar Davis, Suspect Devices
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in thedocumentation
   and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
  -----------------------------------------------------------------------------
   The idea here is to create an automation freindly monitor protocol and debug
   interface. Because this needs to be efficient (and flexible) I have chosen a
   very simple interface which will access and may manipulate a devices public
   variables.

   Basic Syntax for the monitor is
   XXX[?!:] <variable stuff> <CR>
   where:
   XXX is a 3 letter command or variable.
   ! execute command or store value
   ? get value
   :<20 char timestamp>, value returned.


   keywords are described in keywords.h
*/

#ifndef Monitor_h
#define Monitor_h

#include "Common.h"
class Monitor;
extern Monitor monitor;

#include "Machine.h"
#include "Adafruit_SleepyDog.h"
#include "Reset.h"

#include "TaskScheduler.h"
#include <stdio.h>
#include <stdarg.h>

// fix this for processor the stubbed here is for the AVR
#define EEPROM_NOT_SET 0xff
#ifndef eeprom_read_byte
#define eeprom_read_byte(x) (0xff)
#endif
#ifndef eeprom_write_byte
# define eeprom_write_byte(x,v) (v)
#endif

#include "keywords.h"

#define MONITOR_SERIAL_NUMBER_CHARS 20

enum severityIndex {
  SEV_NONE, SEV_FATAL, SEV_ALERT, SEV_WARN, SEV_INFO, SEV_DEBUG, SEV_LOG
};

#define MAX_RETURN_VALUE 104
#define MAX_MONITOR_LINE_LENGTH (MAX_RETURN_VALUE-6)

typedef void(*actionptr)(uint8_t, uint8_t, char *);
//typedef void(actionfunc)(uint8_t, uint8_t, char *);

extern char actionBuffer[MAX_RETURN_VALUE];
extern actionptr actions[];


#define MAX_PSN_LENGTH 33

//forwards...
//class Monitor;
//extern Monitor monitor;
class Machine;
extern Machine machine;
extern int getFreeMemory();
extern char commandBuffer[];

class Monitor
{
  private:
    uint8_t _debugLevel;
    uint8_t _commandMode;
    uint8_t _logLevel;
    char _psn[MAX_PSN_LENGTH]; // bracket this ... samd processor serial number
    int get_free_memory();

    int lookupIndex(char *key);


  public:
//    Stream * console;
    char * lookupKey(int ndx);

    static char *keyword(int ndx);
    
    /*--------------------------------------------------------------------------------
     * built in callback routines.
     *-------------------------------------------------------------------------------*/
    
    static void ACK (uint8_t kwIndex, uint8_t verb, char *args) {
        monitor.update("ACK","");
    }

    static void NOP(uint8_t kwIndex, uint8_t verb, char* args) {
      monitor.update("NAK", "%s is not implemented!", monitor.lookupKey(kwIndex));
    }

    static void DVL(uint8_t kwIndex, uint8_t verb, char* args) {
      if (verb == '!') {
        monitor.setDebugLevel(atoi(args));
      }
      monitor.update("DVL", "%d", monitor.debugLevel());
    }

    static void MEM (uint8_t kwIndex, uint8_t verb,char *args) {
        monitor.update("MEM","%d",getFreeMemory());
    }
    static void HWV (uint8_t kwIndex, uint8_t verb,char *args) {
        monitor.update("HWV","%d",monitor.unitHardwareVersion());
    }
    
    static void CMD(uint8_t kwIndex, uint8_t verb, char *args) {
        if (verb=='!') {
            bool setIt = (atoi(args));
            monitor.setCommandMode(setIt);
        }
        monitor.update("CMD","%s",monitor.commandMode()?"ON":"OFF");
    }
    
    static void SWV(uint8_t kwIndex, uint8_t verb, char *args) {
        monitor.update("SWV","%s",FIRMWAREREV);
    }
    
    static void SSN(uint8_t kwIndex, uint8_t verb, char *args) {
        monitor.update("SSN","%s",monitor.unitSerialNumber());
    }
    
    static void GIT(uint8_t kwIndex, uint8_t verb, char *args) {
        monitor.update("GIT","%s",GIT_VERSION);
    }
    
    static void RST(uint8_t kwIndex, uint8_t verb, char *args) {
        monitor.warn("Rebooting!");
        MONITOR_UART.end();
        Watchdog.enable(150);
    }
    
    static void BLD(uint8_t kwIndex, uint8_t verb, char *args) {
        monitor.warn("Jumping to Bootloader!");
        initiateReset(10);
    }
    
    // stubb
    static void HLP(uint8_t kwIndex, uint8_t verb, char *args) {
        monitor.update("NAK","HLP requires multi line data (MLD)");
    }
    
    Monitor();

    static void run();

    char *unitSerialNumber() {
      return _psn;
    };
    
    int unitHardwareVersion() {
      return 0;
    };
    
    
    void update(const char *, const char *format, ...);
    void fatal(int, const char *format, ...);
    void alert(const char *format, ...) { va_list blarg; update("ALT", format, blarg); va_end(blarg);};
    void warn(const char *format, ...) { va_list blarg; update("WAR", format, blarg); va_end(blarg);};
    void log(const char *format, ...) { va_list blarg; update("LOG", format, blarg); va_end(blarg);};
    // void info(int);
    void debug(const char *format, ...);
    void setDebugLevel(uint8_t lvl) {
      //eeprom_write_byte(DEBUG_LEVEL_EEPROM_ADDR, (_debugLevel = lvl));
        _debugLevel = lvl;

    }; //FIX
    void setCommandMode(uint8_t mode) {
      _commandMode = mode;
    }
    uint8_t commandMode() {
      return _commandMode;
    }
    uint8_t debugLevel() {
        return _debugLevel;// = eeprom_read_byte(DEBUG_LEVEL_EEPROM_ADDR);
    }; //FIX
    void init();
    void registerAction(uint8_t, actionptr);
};
#endif

