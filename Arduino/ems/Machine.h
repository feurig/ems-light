#ifndef Machine_h
#define Machine_h
#include "Common.h"
#include "Monitor.h"
#include "TaskScheduler.h"

class Machine;
extern Machine machine;

static void STC (uint8_t kwIndex, uint8_t verb,char *args);
static void TMR (uint8_t kwIndex, uint8_t verb,char *args);
static void TMS (uint8_t kwIndex, uint8_t verb,char *args);
static void TRM (uint8_t kwIndex, uint8_t verb,char *args);
static void systemInCharge();

class Machine
{
private:
    Task monitorTask;
    Task machineTask;
    
public:
    int previousState;
    int currentState;
    int timerSetting;
    bool timerIsRunning;
    int timeRemaining;
    bool inSafeStateToRun;
    
    Machine() : currentState(STATE_OFF) {}
    Scheduler todolist;
    void init();
    void setPreviousState(int newState); // {currentState=newState;};
    void setState(int newState); // {currentState=newState;};
    int state() { return currentState; };
    const char * stateName(int) ;
    
protected:

};

#endif
