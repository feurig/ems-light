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


#include "Configuration.h"
#include "Monitor.h"

class SafetyThird;
extern SafetyThird safety;
#define RED_FULL_ON (255 * (1 << (ANALOG_WRITE_RESOLUTION - 8)))
#define RED_OFF 0

// preprocessor logic for core specific WDT implimentation here....
#include <Adafruit_SleepyDog.h>

static void RED (uint8_t kwIndex, uint8_t verb,char *args) ;

class SafetyThird {
    
private:
    bool _eStop;
    bool _prevEStop;
    bool _prevLightCurtain;
    // put the contactor variables back here
    // mebby look at the light curtain as well
    Task _doDoRunRun;
    
public:

    bool _lightCurtain;
    bool _lightCurtainReset;

#ifdef LIGHT_CURTAIN_START_PIN
    static void LCS(uint8_t kwIndex, uint8_t verb, char *args) {
        //
        // set logic here
        //
        monitor.update("LCS","%s",safety._lightCurtain?"ON":"OFF");
        safety.first();
    }

    static void LCR(uint8_t kwIndex, uint8_t verb, char *args) {
        if (verb=='!') {
            safety._lightCurtainReset = (atoi(args));
            digitalWrite(LIGHT_CURTAIN_START_PIN,safety._lightCurtainReset);
        }
        safety.first();
        monitor.update("LCR","%s",safety._lightCurtainReset?"ON":"OFF");
    }
#endif
    
    static void run() {safety.first();}
    
    void first () { // could also be called paranoia.

#ifdef E_STOP_PIN
        _eStop = (digitalRead(E_STOP_PIN)==E_STOP_PULLED );
#else
        _eStop = false;
#endif

#ifdef LIGHT_CURTAIN_OSDD_PIN
        _lightCurtain = (digitalRead(LIGHT_CURTAIN_OSDD_PIN) == LIGHT_CURTAIN_NOT_CLEAR);
#else
        _lightCurtain = false;
#endif
        if ((machine.inSafeStateToRun)
            && (_eStop)
            )
        {
            monitor.update("STP","1 ESTOP Pushed: Initiating Emergency Stop");
            machine.setState(STATE_E_SHUTDOWN);
        }
        if (_eStop!=_prevEStop) {
            _eStop = _prevEStop;
            if (_eStop){
                // probably should warn or alert!
                monitor.update("STP","1 ESTOP Pushed: Contactors are OFF");
#ifdef K1_PIN
                digitalWrite(K1_PIN,KONTACTOR_OFF);
#endif
            } else {
                monitor.update("STP","0 E-STOP Released: Contactors are ON");
#ifdef K1_PIN
                digitalWrite(K1_PIN,KONTACTOR_ON);
#endif
            }
        }
        //
        // this is rough... Fix when there is hardware to test with
        //
        if(_lightCurtain!=_prevLightCurtain) {
            if (_lightCurtain) {
#ifdef K1_PIN
                digitalWrite(K1_PIN,KONTACTOR_OFF);
#endif
            } else {
#ifdef K1_PIN
                digitalWrite(K1_PIN,KONTACTOR_OFF);
#endif≥                
            }
            monitor.update("LCS","%s",_lightCurtain?"ON":"OFF");
        }
        _prevLightCurtain=_lightCurtain;
        _prevEStop=_eStop;
    }
    uint redLightVal; // red led value prolly bad style

    void redLight(int newVal) {
        if(newVal!=redLightVal) {
            redLightVal=newVal;
            monitor.update("RED","%d",redLightVal);
        }
#ifdef SAFETY_LAMP_PIN
        analogWrite(SAFETY_LAMP_PIN, safety.redLightVal * (1 << (ANALOG_WRITE_RESOLUTION - 8) ) );
#endif
    }

    void init() {
#ifdef E_STOP_PIN
        pinMode (E_STOP_PIN, INPUT_PULLUP);
#endif

        //  Might move this to machine or other more generic place.
        //  http://www.arduino.org/learning/reference/analogwriteresolution
        analogWriteResolution(ANALOG_WRITE_RESOLUTION);
#ifdef LIGHT_CURTAIN_RESET_PIN
        _lightCurtainReset = true;
        digitalWrite(LIGHT_CURTAIN_START_PIN,LIGHT_CURTAIN_START);
#else
        _lightCurtainReset = false;
#endif
        _prevLightCurtain=false;
        _prevEStop=false;
        
        redLight(redLightVal=RED_OFF);
        _doDoRunRun.set(77L, TASK_FOREVER, &run);
        machine.todolist.addTask(_doDoRunRun);
        _doDoRunRun.enable();
        
        monitor.registerAction(_RED_, &RED);
    }
};

static void RED (uint8_t kwIndex, uint8_t verb,char *args) {
    // monitor update?
    safety.redLight(atol(args));

}


#endif

