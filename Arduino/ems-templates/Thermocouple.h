/******************************************************************
 * Thermocouple.h - type description for temperature measurement.
 * 
 * this should be generalized into a temperature sensor.
 *
 */

 #ifndef THERMOCOUPLE_H
 #define THERMOCOUPLE_H


#include <SPI.h>
#include "Adafruit_MAX31855.h"

#include "Common.h"
#include "TaskScheduler.h"
#include "Monitor.h"
#include "Machine.h"

static double CtoF(double C) {
  return (9.0*C)/5.0 + 32.0;
}

// output temperature values when they have changed
static void readPlate1Temp();
static void readPlate2Temp();
static void readAmbientTemp();
static void TS1 (uint8_t kwIndex, uint8_t verb,char *args); 
static void TS2 (uint8_t kwIndex, uint8_t verb,char *args);
static void AMT (uint8_t kwIndex, uint8_t verb,char *args);

class Thermocouples {
  private:
    Adafruit_MAX31855 _therm1;
    double _temp1;
    bool _temp1changed;

    Adafruit_MAX31855 _therm2;
    double _temp2;
    bool _temp2changed;

    // ambient temp is made up of the avg ambient temp from each thermocouple
    double _ambientTemp;
    bool _ambChanged;
    
    Task _temp1Read;
    Task _temp2Read;
    Task _ambientTempRead;
    
  public:
    Thermocouples() : _therm1(TS1_CS_PIN), _therm2(TS2_CS_PIN) {}
    Adafruit_MAX31855* therm1()  {return &_therm1;}
    Adafruit_MAX31855* therm2()  {return &_therm2;}

    // temperature 1 changes
    bool updateTemp1() {
      double  tol = 1;
      double temp = therm1()->readFarenheit();
      _temp1changed = fabs(_temp1 - temp) >= tol;
      if (_temp1changed)
        _temp1 = temp;
      return _temp1changed;
    }

    void setTemp1Changed(bool v) {
      _temp1changed = v;
    }

    // temperature 2 changes
    bool updateTemp2() {
      double  tol = 1;
      double temp = therm2()->readFarenheit();
      _temp2changed = fabs(_temp2 - temp) >= tol;
      if (_temp2changed)
        _temp2 = temp;
      return _temp2changed;
    }

    void setTemp2Changed(bool v) {
        _temp2changed = v;
    }

    // Check for Ambient temperature changes
    bool updateAmbientTemp() {
      double tol = 1;
      // Farenheit temperature
      double avgTemp = CtoF((therm1()->readInternal() + therm2()->readInternal())/2.0);
      _ambChanged = fabs(_ambientTemp - avgTemp) >= tol;
      if (_ambChanged) {
        _ambientTemp = avgTemp;
      }
      return _ambChanged;
    }

    void setAmbChanged(bool v) {
      _ambChanged = v;
    }
    
    double getTemp1() { return _temp1; }
    double getTemp2() { return _temp2; }
    double getAmb()   { return _ambientTemp; }
    
    bool updateTemp2(double temp);
    
    void init() {
      monitor.registerAction(_TS1_, &TS1);
      monitor.registerAction(_TS2_, &TS2);
      monitor.registerAction(_AMT_, &AMT);

      // set tasks for sceduled reads
      _temp1Read.set(100L, TASK_FOREVER, &readPlate1Temp);
      machine.todolist.addTask(_temp1Read);
      _temp1Read.enable();
      
      _temp2Read.set(100L, TASK_FOREVER, &readPlate2Temp);
      machine.todolist.addTask(_temp2Read);
      _temp2Read.enable();
      
      _ambientTempRead.set(100L, TASK_FOREVER, &readAmbientTemp);
      machine.todolist.addTask(_ambientTempRead);
      _ambientTempRead.enable();
    }

    
};

extern Thermocouples thermocouple;

/*----------------------------------------------------------------------
 *  Schedule Callbacks
 *--------------------------------------------------------------------*/
// return the temp only if the plate temp has changed
static void readPlate1Temp() { 
    if (thermocouple.updateTemp1()) {
        double _f = thermocouple.getTemp1();
        monitor.update("TS1", "%d.%.02d", int(_f),int(abs(_f - int(_f))*100));
        thermocouple.setTemp1Changed(false);
    }
}

static void readPlate2Temp() { 
    if (thermocouple.updateTemp2()) {
        double _f = thermocouple.getTemp2();
        monitor.update("TS2", "%d.%.02d", int(_f),int(abs(_f - int(_f))*100));
        thermocouple.setTemp2Changed(false);
    }
}

static void readAmbientTemp() {
  if (thermocouple.updateAmbientTemp()) {
        double _f = thermocouple.getAmb();
        monitor.update("AMT", "%d.%.02d", int(_f),int(abs(_f - int(_f))*100));
        thermocouple.setAmbChanged(false);
  }
}

/*----------------------------------------------------------------------
 *  Monitor Callbacks
 *--------------------------------------------------------------------*/
    
// unconditionally return temp
static void TS1 (uint8_t kwIndex, uint8_t verb,char *args) {
    double _f = thermocouple.getTemp1();
    monitor.update("TS1", "%d.%.02d", int(_f),int(abs(_f - int(_f))*100));
}

static void TS2 (uint8_t kwIndex, uint8_t verb,char *args) {
    double _f = thermocouple.getTemp2();
    monitor.update("TS2", "%d.%.02d", int(_f),int(abs(_f - int(_f))*100));
}

static void AMT (uint8_t kwIndex, uint8_t verb,char *args) {
    double _f = thermocouple.getAmb();
    monitor.update("AMT", "%d.%.02d", int(_f),int(abs(_f - int(_f))*100));
}


 #endif // THERMOCOUPLE_H
