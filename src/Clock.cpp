/*-------------------------------------------------------------------Clock.cpp
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
 * The clock module is about time.
 * This includes
 * An RTC backed (if avaliable) time module for timestamps
 * A shcedular for most tasks.
 */

#include "Monitor.h"
#include "Clock.h"


/* FIX ME>>>>>
 * notes on sync weirdness.
 *
 * This class grew out of a system which used external RTC that had to share
 * a buss with all sorts of sensors.
 *
 * Rather than hog the buss a running seconds count was maintained (unixtime)
 * and the time variables were updated and the rtc referenced once a minute.
 *
 * With an internal rtc this gets a little krufty.
 *
 * On the other hand a less than perfect clock can be implimented on systems
 * w/o access to RTCs by counting seconds using whatever millis is using.
 *
 * this is not currently implimented but we will leave the cruft so it can be.
 *
 */

/* Create an rtc object mebby should be private to the class JOE?? */

// add preprocessor code here for non M0 systems.
#ifdef ARDUINO_SAMD_ZERO
RTCZero rtc;
#endif
/* calculate build date and time (this should be referenceable from monitor for SWV ... */
const byte build_seconds = ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0');
const byte build_minutes = ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0');
const byte build_hours = ((__TIME__[0] - '0') * 10 + __TIME__[1] - '0');


const byte build_day = ((__DATE__[4] >= '0') ? (__DATE__[4] - '0') * 10 : 0) + \
        (__DATE__[5] - '0') ;
const byte build_month =  ( \
        (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n') ?  1 : \
        (__DATE__[0] == 'F') ?  2 : \
        (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r') ?  3 : \
        (__DATE__[0] == 'A' && __DATE__[1] == 'p') ?  4 : \
        (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y') ?  5 : \
        (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n') ?  6 : \
        (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l') ?  7 : \
        (__DATE__[0] == 'A' && __DATE__[1] == 'u') ?  8 : \
        (__DATE__[0] == 'S') ?  9 : \
        (__DATE__[0] == 'O') ? 10 : \
        (__DATE__[0] == 'N') ? 11 : \
        (__DATE__[0] == 'D') ? 12 : \
        /* error default */  99 \
    );
const byte build_year = ( \
        (__DATE__[ 9] - '0') *   10 + \
        (__DATE__[10] - '0') \
    );


Clock clock;


void Clock::init()

{

  rtc.begin(); // initialize RTC

  // if the rtc is set in the early 00s Set the and date to build time initially
 //  otherwise assume that the battery was keeping it running appropriately.
  if (rtc.getYear()>1){
     rtc.setTime(build_hours,build_minutes,build_seconds);
     rtc.setDate(build_day, build_month, build_year);
  }

  monitor.debug("Clock module initialized");
  monitor.registerAction(_TIM_, &TIM);
  monitor.registerAction(_NOW_, &NOW);
}

// bracket with pre-processor defs ....
bool Clock::RTCIsRunning(void) {
    return (RTC->MODE2.CTRL.reg & RTC_MODE2_CTRL_ENABLE);
}

const uint8_t daysInMonth [] { 31,28,31,30,31,30,31,31,30,31,30,31 };

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t yy, uint8_t mm, uint8_t dd) {
    if (yy >= 2000)
        yy -= 2000;
    uint16_t days = dd;
    for (uint8_t i = 1; i < mm; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (mm > 2 && yy % 4 == 0)
        ++days;
    return days + 365 * yy + (yy + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t hh, uint8_t mm, uint8_t ss) {
    return ((days * 24L + hh) * 60 + mm) * 60 + ss;
}

static uint8_t conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

void Clock::calcUnixTime(void) {
    uint32_t t;
    uint16_t days = date2days(yOff, m, d);
    t = time2long(days, hh, mm, ss) + SECONDS_FROM_1970_TO_2000 ;
    unixtime = t;
}

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

void Clock::set(time_t t) {
    rtc.setEpoch((uint32_t)t);
}

void Clock::set(const char * dateString) {
    //TIM:%02d/%02d/%04d %02d:%02d:%02d"
    //TIM:mm/dd/yyyy hh:mm:ss
        //0123456789012345678901234
    uint8_t seconds,minutes,hours,days,leap,month;
    long int yearoff;
    char dateStringBuffer[24];
    strncpy(dateStringBuffer,dateString,23);
    dateStringBuffer[2]=dateStringBuffer[5]=dateStringBuffer[10]
    =dateStringBuffer[13]=dateStringBuffer[16]=dateStringBuffer[19]='\0';
    seconds = atoi(dateStringBuffer+17);
    minutes = atoi(dateStringBuffer+14);
    hours = atoi(dateStringBuffer+11);;
    days = atoi(dateStringBuffer+3);
    month = atoi(dateStringBuffer);
    yearoff = atoi(dateStringBuffer+6) - (2000);
    monitor.debug("?:%02d/%02d/%04d %02d:%02d:%02d",month,days,yearoff+(2000),hours,minutes,seconds);
    rtc.setTime(hours,minutes,seconds);
    rtc.setDate(days, month, yearoff);
}

uint8_t Clock::dayofweek() /* 0 = Sunday */
{   int y=year();
    int m=month();
    int d=day();
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return (uint8_t) ((y + y/4 - y/100 + y/400 + t[m-1] + d) % 7);
}

// populate running values from RTC
void Clock::sync() {
    ss = rtc.getSeconds();
    rtcisrunning = 1; //FIXME!!!
    mm = rtc.getMinutes();
    hh = rtc.getHours();
    d = rtc.getDay();
    m = rtc.getMonth();
    yOff = rtc.getYear();
#if DEBUG_SYNC
    monitor.debug("RTC: SYNC mm=%d,ss=%d",mm,ss);
#endif

}

void Clock::run() {

}


void timeStamp ( char *buffer ) {
    sprintf(buffer,"%02d/%02d/%04d %02d:%02d:%02d",
              clock.month(), clock.day(), clock.year(),
              clock.hour(), clock.minute(), clock.second());
}
