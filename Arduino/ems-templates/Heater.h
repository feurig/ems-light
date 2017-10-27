/**************************************************
 * Heater.h - Describe the heater interface
 * 
 *
 */
 #ifndef HEATER_H
 #define HEATER_H

#include "Configuration.h"
#include "TaskScheduler.h"
#include "Monitor.h"
#include "Machine.h"

#include "Thermocouple.h"

// move to Configuration.h

#define HEATER_OFF 0
#define HEATER_OUT_MAX 255
#define HEATER_OUT_MIN 0
#define HEATER_MAX_TEMP 280.00
#define HEATER_PID_SAMPLE_TIME 100L

static void run();
static void CheckHeaters();

static void TMP (uint8_t kwIndex, uint8_t verb,char *args);
static void HSW (uint8_t kwIndex, uint8_t verb,char *args);

static void H1P (uint8_t kwIndex, uint8_t verb,char *args);
static void H1I (uint8_t kwIndex, uint8_t verb,char *args); 
static void H1D (uint8_t kwIndex, uint8_t verb,char *args); 

static void H2P (uint8_t kwIndex, uint8_t verb,char *args);
static void H2I (uint8_t kwIndex, uint8_t verb,char *args); 
static void H2D (uint8_t kwIndex, uint8_t verb,char *args); 


const double SampleTimeInSec = ((double)HEATER_PID_SAMPLE_TIME)/1000;



class Heater {
  private:
    
    Task runHeaters;
   
  public:
    double setPoint;
    bool isOn;
    int H1Output, H2Output;
    double H1SetPoint, H1LastInput, H1ITerm;
    double H2SetPoint, H2LastInput, H2ITerm;
    
    //TODO: P,I,D 'accessors'
    double P1, I1, D1, P2, I2, D2;
    

    void runThermostat(){
        double input, output, error, i, d;
        
        i = I1 * SampleTimeInSec;
        d = D1 / SampleTimeInSec;
        input = thermocouple.getTemp1();
        error = H1SetPoint - input;

        H1ITerm += (i * error);
        if(H1ITerm > HEATER_OUT_MAX) {
            H1ITerm = HEATER_OUT_MAX;
        }
        else if(H1ITerm < HEATER_OUT_MIN){
            H1ITerm= HEATER_OUT_MIN;
        }
        
        output = P1 * error + H1ITerm - d * (input - H1LastInput);
 
        if(output > HEATER_OUT_MAX){
            output = HEATER_OUT_MAX;
        }
        else if(output < HEATER_OUT_MIN) {
            output = HEATER_OUT_MIN;
        }
        H1Output = (int) output;
        
        if(isOn) {
            analogWrite(HTR1_PIN,H1Output);
        }
        
        H1LastInput = input;
        
        input = thermocouple.getTemp2();
        error = H2SetPoint - input;
        i= I2 * SampleTimeInSec;
        d= D2 / SampleTimeInSec;
        
        H2ITerm += (i * error);
        if(H2ITerm > HEATER_OUT_MAX) {
            H2ITerm = HEATER_OUT_MAX;
        }
        else if(H2ITerm < HEATER_OUT_MIN){
            H2ITerm= HEATER_OUT_MIN;
        }
        
        output = P2 * error + H2ITerm - d * (input - H2LastInput);
        if(output > HEATER_OUT_MAX){
            output = HEATER_OUT_MAX;
        }
        else if(output < HEATER_OUT_MIN) {
            output = HEATER_OUT_MIN;
        }
        
        H2Output = (int) output;
        
        if(isOn) {
            analogWrite(HTR2_PIN,H2Output);
        }
        
        H2LastInput = input;
        
    };
    
    void init() {
      pinMode(HTR1_PIN, OUTPUT);
      pinMode(HTR2_PIN, OUTPUT);
      analogWrite(HTR1_PIN, HEATER_OFF); // number foo
      analogWrite(HTR2_PIN, HEATER_OFF);
        
        
      P1=1;
      I1=0.05;
      D1=0.25;
        
      P2=1;
      I2=0.05;
      D2=0.25;
      
      isOn=0;
      setPoint=0.0;
        
      // register monitor actions
      monitor.registerAction(_TMP_, &TMP);
      monitor.registerAction(_HSW_, &HSW);
      
      monitor.registerAction(_H1P_, &H1P);
      monitor.registerAction(_H1I_, &H1I);
      monitor.registerAction(_H1D_, &H1D);

      monitor.registerAction(_H2P_, &H2P);
      monitor.registerAction(_H2I_, &H2I);
      monitor.registerAction(_H2D_, &H2D);

      // TODO: check to see if turning on the heater will change the temp.
 
      // set tasks for sceduled reads
      runHeaters.set(HEATER_PID_SAMPLE_TIME, TASK_FOREVER, &run);
      machine.todolist.addTask(runHeaters);
      runHeaters.enable();
    }

    void StopHeaters() {
        isOn=0;
        CheckHeaters();
    }
    
   

};

Heater heater;

static void run() {
       CheckHeaters();
 
}


// check heaters and change state if necessary
// Check the temperature and turn the heater off if the target temp reached.
// Turn the heater on of the temp is below he target.
// TODO: add PID functionality here.
static void CheckHeaters() {
    double t;
    t=thermocouple.getTemp1();
    if (t>HEATER_MAX_TEMP) {
        heater.isOn=false;
        monitor.warn("overtmp d.%.02d", FAKE_A_FLOAT(t));
    }
    t=thermocouple.getTemp2();
    if (t>HEATER_MAX_TEMP) {
        heater.isOn=false;
        monitor.warn("overtmp d.%.02d", FAKE_A_FLOAT(t));
    }
    if (heater.isOn){
        heater.runThermostat();
    } else {
        analogWrite(HTR1_PIN, HEATER_OFF);
        analogWrite(HTR2_PIN, HEATER_OFF);
    }
}




// define or query P, I, and D for heater 1

static void TMP(uint8_t kwIndex, uint8_t verb, char* args) {
    if (verb == '!') {
        heater.H1SetPoint = heater.H2SetPoint = heater.setPoint = atof(args);
    }
    monitor.update("TMP", "%d.%.02d", FAKE_A_FLOAT(heater.setPoint));
}

static void HSW(uint8_t kwIndex, uint8_t verb, char* args) {
    //monitor.debug("atoi('ON')=%d",atoi("ON"));
    if (verb == '!') {
        heater.isOn = atoi(args);
    }
    monitor.update("HSW", "%d", heater.isOn);
}

static void H1P(uint8_t kwIndex, uint8_t verb, char* args) {
    if (verb == '!') {
        heater.P1 = atof(args);
    }
    monitor.update("H1P", "%d.%.02d", FAKE_A_FLOAT(heater.P1));
}

static void H1I(uint8_t kwIndex, uint8_t verb, char* args) {
    if (verb == '!') {
        heater.I1 = atof(args);
   }
    monitor.update("H1I", "%d.%.02d", FAKE_A_FLOAT(heater.I1));
}
static void H1D(uint8_t kwIndex, uint8_t verb, char* args) {
    if (verb == '!') {
        heater.D1 = atof(args);
    }
    monitor.update("H1D", "%d.%.02d", FAKE_A_FLOAT(heater.D1));
}

static void H2P(uint8_t kwIndex, uint8_t verb, char* args) {
    if (verb == '!') {
        heater.P2 = atof(args);
    }
    monitor.update("H2P", "%d.%.02d", FAKE_A_FLOAT(heater.P2));
}

static void H2I(uint8_t kwIndex, uint8_t verb, char* args) {
    if (verb == '!') {
        heater.I2 = atof(args);
    }
    monitor.update("H2I", "%d.%.02d", FAKE_A_FLOAT(heater.I2));
}

static void H2D(uint8_t kwIndex, uint8_t verb, char* args) {
    if (verb == '!') {
        heater.D1 = atof(args);
    }
    
    monitor.update("H2D", "%d.%.02d", FAKE_A_FLOAT(heater.D2));
}

 #endif // HEATER_H
