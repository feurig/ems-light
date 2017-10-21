/*-----------------------------------------------------------------SafetyThird.h
 *
 * *********************** Don't put safety third !!!! *************************
 *
 * This is a reminder not to put safety third like your workplace does.
 * (after corporate profits and management whim)
 *
 * Author: Donald Delmar Davis, Suspect Devices
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
 *-----------------------------------------------------------------------------*/
/*
 * The safteyThird module is a place for items relating to the safety of machines.
 * Examples include:
 * Watchdog timer (a mechanism for keeping code from running away from its job)
 * K1, K2 ( in large machinery a pair of Kontactors designed to drop power )
 * E-stop ( A big red button or a lanyard for emergencies)
 * Light Curtain (a device to keep fingers from getting into dangerous areas)
 * red. (lights or other indicators that there may be problems.
 */

#ifndef SafetyThird_h
#define SafetyThird_h


#include "Common.h"
#include "Monitor.h"

class SafetyThird;
extern SafetyThird safety;
#if 0
#define SAFE_STATE_TO_RUN(S) (\
(S)==STATE_SELF_TEST_INIT\
||(S)==STATE_WARM_UP||\
(S)==STATE_NORMAL_OPERATION\
)
#else
#define SAFE_STATE_TO_RUN(S) machine.inSafeStateToRun
#endif

// preprocessor logic for core specific WDT implimentation here....
#include <Adafruit_SleepyDog.h>

static void RED (uint8_t kwIndex, uint8_t verb,char *args) ;

class SafetyThird {

  private:
    bool _eStop;
    bool _prevEStop;
    bool _prevLightCurtain;
    
    Task _doDoRunRun;
    
  public:

    bool _lightCurtain;
    bool _lightCurtainReset;

    static void LCS(uint8_t kwIndex, uint8_t verb, char *args) {
        safety.first();
        monitor.update("LCS","%s",safety._lightCurtain?"ON":"OFF");
    }
    
    static void LCR(uint8_t kwIndex, uint8_t verb, char *args) {
        if (verb=='!') {
            safety._lightCurtainReset = (atoi(args));
            digitalWrite(START_PIN,safety._lightCurtainReset);
        }
        safety.first();
        monitor.update("LCR","%s",safety._lightCurtainReset?"ON":"OFF");
    }
    
    static void run() {safety.first();}
    
    void first () { // could also be called paranoia.
#ifdef CAN_READ_ESTOP
        _eStop = (digitalRead(E_STOP_PIN)==E_STOP_PULLED );
#else 
        _eStop = ! E_STOP_PULLED;
#endif
        _lightCurtain = (digitalRead(OSDD_PIN) == CURTAIN_NOT_CLEAR);
        int state = machine.state();
        if ((SAFE_STATE_TO_RUN (state))
            && !(_eStop)
            && !(_lightCurtain)
            )
        {
            if (!_prevEStop){
                monitor.update("STP","0 E-STOP Released: Contactors are ON");
            }
            if (!_prevLightCurtain){
                monitor.update("LCS","1 WORK AREA CLEARED: Contactors are ON");
            }
            
            digitalWrite(K1_PIN,KONTACTOR_ON);
        }
        if (_eStop){
            digitalWrite(K1_PIN,KONTACTOR_OFF); // in most cases this would already be done
            if (!_prevEStop){
                monitor.update("STP","1 ESTOP Pushed: Contactors are OFF");
            // fatal....
        
            }
        }
        if (_lightCurtain) {
            digitalWrite(K1_PIN,KONTACTOR_OFF); // in most cases this would already be done
            if(!_prevLightCurtain){
                
                monitor.update("LCS","1 WORK AREA NOT CLEAR: Contactors are OFF");
            // safety pause
        
            }
        }
        
        _prevEStop=_eStop;
        _prevLightCurtain=_lightCurtain;
    }
    
    uint8_t redLed; // red led value prolly bad style

    void init() {
        pinMode (E_STOP_PIN, INPUT_PULLUP);
        pinMode (OSDD_PIN, INPUT_PULLUP);
        pinMode (START_PIN, OUTPUT);
        digitalWrite(START_PIN, OK_TO_START);
        pinMode (K1_PIN, OUTPUT);
        digitalWrite(K1_PIN, KONTACTOR_OFF);
        
      //  http://www.arduino.org/learning/reference/analogwriteresolution
      analogWriteResolution(ANALOG_WRITE_RESOLUTION);
      analogWrite(LAMP_PIN, redLed * (1 << (ANALOG_WRITE_RESOLUTION - 8) ) );

        _doDoRunRun.set(77L, TASK_FOREVER, &run);
        machine.todolist.addTask(_doDoRunRun);
        _doDoRunRun.enable();

        monitor.registerAction(_RED_, &RED);
        monitor.registerAction(_LCS_, &LCS);
        monitor.registerAction(_LCR_, &LCR);
    }
//    bool cleanShutdown(); // let modules know if we are starting normally
//    void everythingOff();
//    void run();
};


static void RED (uint8_t kwIndex, uint8_t verb,char *args) {
  safety.redLed = atol(args);
  analogWrite(LAMP_PIN, safety.redLed * (1 << (ANALOG_WRITE_RESOLUTION - 8) ) );
}


#endif

