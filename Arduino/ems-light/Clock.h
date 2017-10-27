/*---------------------------------------------------------------------Clock.h
 * Authors: Joseph Wayne Dumoulin, Donald Delmar Davis, Suspect Devices
 *
 * Liscence: "Simplified BSD License"
 *
 * Copyright (c) 2016, Donald Delmar Davis, Suspect Devices
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in thedocumentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 *
 * 
 *
 */

#ifndef Clock_h
#define Clock_h

#include "Configuration.h"
#include "Monitor.h"
#include "TaskScheduler.h"

#define SECONDS_PER_DAY 86400L
#define SECONDS_FROM_1970_TO_2000 946684800
#define DEBUG_SYNC 0
#define CHARS_IN_TIMESTAMP 20

// add preprocessor code here for non M0 systems.
#include <RTCZero.h>
extern RTCZero rtc;

// forward
class Clock;
extern Clock clock;

void timeStamp(char *);

class Clock
{
private:

   
public:
    
    
    /*----------------------------------------------------------------------
     *  Monitor Callbacks
     *--------------------------------------------------------------------*/
    
    static void TIM (uint8_t kwIndex, uint8_t verb,char *args) {
        static char thetimestamp[CHARS_IN_TIMESTAMP];
        if (verb == '!') {
            clock.set(args);
        }
        timeStamp(thetimestamp);
        monitor.update("TIM","%s",thetimestamp);
    }
    
    static void NOW (uint8_t kwIndex, uint8_t verb,char *args) {
        static char intbuff[25];
        if (verb == '!') {
            clock.set(atol(args));
        }
        ultoa(clock.time(),intbuff,10);
        monitor.update("NOW","%s",intbuff);
        
    }
    

    Clock() {} ;
    void init();
    bool RTCIsRunning();// const { return (rtcisrunning==1); };
    void set(time_t);
    time_t get();
    void set(const char *);
    
// need to generalize this at some point.
#if 0
    uint16_t year()  const  { return 2000 + yOff; }
    uint8_t month()  const  { return m; }
    uint8_t day()    const  { return d; }
    uint8_t hour()   const  { return hh % 24; }
    uint8_t minute() const  { return mm % 60; }
    uint8_t second() const  { return ss % 60; }
    time_t time() const {return unixtime;}
#else
    uint16_t year()  const  { return 2000 + rtc.getYear(); }
    uint8_t month()  const  { return rtc.getMonth(); }
    uint8_t day()    const  { return rtc.getDay(); }
    uint8_t hour()   const  { return rtc.getHours(); }
    uint8_t minute() const  { return rtc.getMinutes(); }
    uint8_t second() const  { return rtc.getSeconds(); }
    time_t time() const {return rtc.getEpoch();}
#endif
    uint8_t dayofweek();
    void tick(void);
    void run(); // this is wrong.
    bool rolledOver(); //??
    void addToCalendar();
    void removeFromCalendar();
    void sync();
    void calcUnixTime();

protected:
    uint8_t yOff, m, d, hh, mm, ss, rtcisrunning;
    time_t unixtime;

};


#endif
