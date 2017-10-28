/*---------------------------------------------------------------------Monitor.h
 * Author: Joseph Wayne Dumoulin, Donald Delmar Davis, Suspect Devices
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
 * The idea here is to create an automation freindly monitor protocol and debug
 * interface. Because this needs to be efficient (and flexible) I have chosen a
 * very simple interface which will access and may manipulate a devices public
 * variables.
 *
 * Basic Syntax for the monitor is
 * XXX[?!:] <variable stuff> <CR>
 * where:
 * XXX is a 3 letter command or variable.
 * ! execute command or store value
 * ? get value
 * :<20 char timestamp>, value returned.
 *
 * keywords are described in keywords.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Monitor.h"
#include "Clock.h"

/*
 * +1 is for the trailing null.
 */
const char keywords[((NKEYWORDS)*3)+1] = KEYWORDS;
#define KWHELPLEN 26

char kwhelp[((NKEYWORDS+1) * KWHELPLEN)+1] = HELPTEXT;
char updateBuffer[MAX_RETURN_VALUE]; //allocate this once use it often.
actionptr actions[NKEYWORDS];


void Monitor::update(const char *data, const char *format, ...) {
    va_list blarg;
    updateBuffer[0]=data[0];updateBuffer[1]=data[1];updateBuffer[2]=data[2];updateBuffer[3]=':';
    timeStamp(updateBuffer+4);
    updateBuffer[23]=POST_TIMESTAMP_DELIMETER;
    va_start(blarg, format);
    vsnprintf(updateBuffer+24, MAX_RETURN_VALUE-24, format, blarg);
    va_end(blarg);
    if (!commandMode()) {
        MONITOR_UART.println(updateBuffer);
        MONITOR_UART.flush();
    }
};

Monitor monitor;

void Monitor::fatal(int newstate, const char *format, ...) {
    va_list blarg;
    update("FTL", format, blarg);
    va_end(blarg);
    //update("FTL","Fatal Error, New State = %s", soh, stateName(soh)); //maybe move this to setstate.
    //sendActionOut();
    //stateChange(soh);
};

void Monitor::debug(const char *format, ...) {
    va_list blarg;
//    if (_debugLevel >= SEV_DEBUG) 
    update("DBG:", format, blarg);
    va_end(blarg);
};




char * samd21m0psn(char * buffer) {

    // https://gist.github.com/mgk/c9ec87436d2d679e5d08
    volatile uint32_t val1, val2, val3, val4;
    volatile uint32_t *ptr1 = (volatile uint32_t *)0x0080A00C;
    val1 = *ptr1;
    volatile uint32_t *ptr = (volatile uint32_t *)0x0080A040;
    val2 = *ptr;
    ptr++;
    val3 = *ptr;
    ptr++;
    val4 = *ptr;
    snprintf(buffer, MONITOR_SERIAL_NUMBER_CHARS ,"%8x%8x%8x%8x", val1, val2, val3, val4);

}

Monitor::Monitor() {

    _debugLevel=EEPROM_NOT_SET;
    samd21m0psn(this->_psn);
    /*
     * insure that all jump table pointers go somewhere
     */
    int k;
   
    for (k=0;k<NKEYWORDS;k++) {
        if (actions[k]==NULL) {
            actions[k]=&NOP;
        }
    }
}
void Monitor::init(){

  
    MONITOR_UART.begin(MONITOR_BAUD);
    while (!MONITOR_UART) 
      delay(1); // wait for USB serial to initialize (BAD IDEA)
//    console=&MONITOR_UART;
    MONITOR_UART.flush();


    
    if (debugLevel()==EEPROM_NOT_SET) {
        setDebugLevel(SEV_WARN); // default level?
    }
    
    monitor.setCommandMode(0);
    
    clock.init(); // needed to timestamp debug statement.
    
    registerAction(_SYN_, &ACK);
    registerAction(_DVL_, &DVL);
    registerAction(_GIT_, &GIT);
    registerAction(_HLP_, &HLP);
    registerAction(_CMD_, &CMD); //
    registerAction(_SSN_, &SSN);
    registerAction(_SWV_, &SWV);
    registerAction(_HWV_, &HWV);
    registerAction(_RST_, &RST);
    registerAction(_BLD_, &BLD);

    //machine.todolist.addTask(__commands);
    //__commands.enable();
//    monitor.update("monitor init");

    monitor.debug("Monitor Init");

}

int commandBufferIndex=0;
bool gotLine=false;
char commandBuffer[84]={}; // rename and use constants.

void Monitor::registerAction(uint8_t ndx, actionptr action){
    if (ndx >=0 && ndx<NKEYWORDS) {
        actions[ndx]=action;
    }
}

void Monitor::run() {
    int index; 
    char key[3];
    char action;
    char *arguments;
    char ch;
    int linlen;
//    if( (monitor.console!=NULL) && monitor.console->available() ){
//       while( monitor.console->available() && commandBufferIndex < MAX_MONITOR_LINE_LENGTH && ((ch=monitor.console->read()) != '\n') && ch !='\r' ) {
//            commandBuffer[commandBufferIndex++] = isprint(ch)?ch:'*';
//            monitor.console->flush();
//        }    monitor.debug("monitor run");

    if( (&MONITOR_UART!=NULL) && MONITOR_UART.available() ){
        while(MONITOR_UART.available() && commandBufferIndex < MAX_MONITOR_LINE_LENGTH && ((ch=MONITOR_UART.read()) != '\n') && ch !='\r' ) {
            commandBuffer[commandBufferIndex++] = isprint(ch)?ch:'*';
            MONITOR_UART.flush();
        }
        
        if (commandBufferIndex && ((ch=='\n')||(ch=='\r'))) {
            while (commandBufferIndex<4) { commandBuffer[commandBufferIndex++]='*';};
            // things get questionable if there are less than 4 characters.
            commandBuffer[commandBufferIndex]='\0';
            commandBufferIndex=0;
            gotLine=true;
        }
    }

    if (gotLine) {
        key[0]=toupper(commandBuffer[0]);
        key[1]=toupper(commandBuffer[1]);
        key[2]=toupper(commandBuffer[2]);
        action=commandBuffer[3];
        arguments=commandBuffer+4;
        index=monitor.lookupIndex(key);
        if (index>=0&&index<NKEYWORDS) {
            actions[index](index,action,arguments);
        } else {
            monitor.update("NAK","%c%c%c[%c] is not understood HLP? for a list of keywords",
                                   key[0],key[1],key[2],action); // better than no response...
        }
        gotLine=false;
    }    
}

int Monitor::lookupIndex(char *key) {
    int ndx=0;
    int ptr=0;
    while (ndx<NKEYWORDS) {
        if (   ( keywords[ptr]==toupper(key[0]) ) 
            && ( keywords[ptr+1] == toupper(key[1]) ) 
            && ( keywords[ptr+2] == toupper(key[2]) )
            ) { return ndx;
        } else {
            ndx++;
            ptr+=3;
        }
    }
    return -1;
}


char keyBuff[4]; //fix or move this ???.

char * Monitor::lookupKey(int ndx) {
    int ptr=0;
    keyBuff[0]=keyBuff[1]=keyBuff[2]='?';
    if (ndx >=0 && ndx<NKEYWORDS) {
        ptr=ndx*3;
        keyBuff[0]=(keywords[ptr]); 
        keyBuff[1]=(keywords[ptr+1]); 
        keyBuff[2]=(keywords[ptr+2]); 
        keyBuff[3]='\0';
    }
        
    return keyBuff;
}



extern "C" char *sbrk(int i);

int getFreeMemory()
{

    //   avr specific
    //    extern int __heap_start, *__brkval;
    //    int v;
    //    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    //    return 0;
    //https://learn.adafruit.com/adafruit-feather-m0-basic-proto/adapting-sketches-to-m0
    char stack_dummy = 0;
    return &stack_dummy - sbrk(0);
    
}







