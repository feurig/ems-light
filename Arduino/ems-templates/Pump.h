/*************************************
 * Pump.h - Manage the hydraulic pump.
 */
#ifndef PUMP_H
#define PUMP_H

#include "Configuration.h"
#include "TaskScheduler.h"
#include "Monitor.h"
#include "Machine.h"
#include "Clock.h"

// move to common
#define PUMP_ON HIGH
#define PUMP_OFF LOW
#define MAX_PUMP_RUN_SECONDS 5
#define GOING_UP LOW
#define GOING_DOWN HIGH
#define SOLENOID_OFF LOW

static void runThePump();
static void readPumpPressure();

static void PMS (uint8_t kwIndex, uint8_t verb,char *args);
static void MUP (uint8_t kwIndex, uint8_t verb,char *args);
static void MDN (uint8_t kwIndex, uint8_t verb,char *args);

static void PS1 (uint8_t kwIndex, uint8_t verb,char *args);
static void SOL (uint8_t kwIndex, uint8_t verb,char *args);

class Pump {
private:
    Task checkPressure;
    Task runPump;
    bool wereWeGoingDown;
    bool wasTheMotorOn;
    long int startedAt;
    bool motorOn;
    bool goingDown;
public:
    int pressure;
    bool pressureChanged;
    
    void motor(bool state) {

        motorOn=state;
        
        if((motorOn==PUMP_ON) && (wasTheMotorOn==PUMP_OFF))
        {
            startedAt=clock.second();
            
        }
        digitalWrite(PUMP_PIN, motorOn?PUMP_ON:PUMP_OFF);
        if (motorOn != wasTheMotorOn) {
            monitor.update("PMS", "%d", motorOn);
        }
        wasTheMotorOn=motorOn;
    }
    
    bool isMotorOn() {return motorOn;}
    
    bool areWeGoingDown() {return goingDown;}
    
    
    void solenoid(bool state) {
        goingDown=state;
        digitalWrite(SOLENOID_PIN, goingDown?GOING_DOWN:GOING_UP);
        if (goingDown != wereWeGoingDown) {
            monitor.update("SOL", "%d", goingDown);
        }
        goingDown = wereWeGoingDown;
        
        
    }
    
    void init() {
        

        pinMode(PUMP_PIN, OUTPUT);
        digitalWrite(PUMP_PIN, PUMP_OFF);
        pinMode(SOLENOID_PIN, OUTPUT);
        digitalWrite(SOLENOID_PIN, GOING_UP);
        StopPump();
        // register monitor commands
        monitor.registerAction(_PMS_, &PMS);
        monitor.registerAction(_PS1_, &PS1);
        monitor.registerAction(_SOL_, &SOL);
        monitor.registerAction(_MUP_, &MUP);
        monitor.registerAction(_MDN_, &MDN);
        
        checkPressure.set(100L, TASK_FOREVER, &readPumpPressure);
        machine.todolist.addTask(checkPressure);
        checkPressure.enable();
        
        runPump.set(100L, TASK_FOREVER, &runThePump);
        machine.todolist.addTask(runPump);
        runPump.enable();
        
    }
    
    void Press() {
        solenoid(GOING_UP);
        motor(PUMP_ON);
        CheckPump(); // ??
    }
    
    void Release() {
        solenoid(GOING_DOWN);
        motor(PUMP_ON);
        CheckPump();
        
    }
    
    void CheckPump() {
        if((motorOn) && ((clock.second()-startedAt) > MAX_PUMP_RUN_SECONDS)){
                monitor.debug("Motor run timed out");
            motor(PUMP_OFF);
        }
        
        // check here for the home switch.
        
    }
    
    void StopPump()  {
        motor(PUMP_OFF);
        solenoid(SOLENOID_OFF);
    }
    
// 'accessors'
    bool updatePressure() {
        int tol = 50; // number foo bad move to macro
        
        int thisReading = analogRead(PS1_APIN);
        if (thisReading < 224) {
            pressure=0;
        } else {
            pressure=(thisReading-200)*10;
        }
        pressureChanged = abs(pressure - pressure) >= tol;
        if (pressureChanged)
            pressure = pressure;
        return pressureChanged;
    }
    
    void setPressureChanged(bool v) {
        pressureChanged = v;
    }
};


Pump pump;

static void runThePump() {
    pump.CheckPump();
}

static void readPumpPressure() {
    if (pump.updatePressure()) {
        monitor.update("PS1", "%d", pump.pressure);
        pump.setPressureChanged(false);
    }
}

// should also take on as a value

static void PMS (uint8_t kwIndex, uint8_t verb,char *args) {
    if (verb == '!') {
        pump.motor((bool) atoi(args));
    } else {
        monitor.update("PMS", "%d", pump.isMotorOn());
    }
}

// should also take on as a value
static void SOL (uint8_t kwIndex, uint8_t verb,char *args) {
    if (verb == '!') {
        pump.solenoid((bool)atoi(args));
    } else {
        monitor.update("SOL", "%d", pump.areWeGoingDown());
    }
}

static void MUP (uint8_t kwIndex, uint8_t verb,char *args) {
    monitor.update("ACK","Going up");
    pump.Press();
}

static void MDN (uint8_t kwIndex, uint8_t verb,char *args) {
    monitor.update("ACK", "Going down");
    pump.Release();
}


static void PS1 (uint8_t kwIndex, uint8_t verb,char *args) {
    monitor.update("PS1", "%d", pump.pressure);
}


#endif // PUMP_H

