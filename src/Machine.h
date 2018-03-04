#ifndef Machine_h
#define Machine_h
#include "Configuration.h"
#include "Monitor.h"
#include "TaskScheduler.h"
// This stuff probably belongs in Machine.h

#define STATENAMELEN 25

enum machine_states {
    STATE_OFF,
    STATE_SELF_TEST_INIT,
    STATE_WARM_UP,
    STATE_RUN_MODE,
    STATE_SHUTDOWN,
    STATE_POST_FAILURE,
    STATE_E_SHUTDOWN,
    STATE_MANUAL_MODE,
    STATE_DIAGNOSTICS_MODE,
    STATE_UNKNOWN_STATE,
};

#define NSTATES (STATE_UNKNOWN_STATE)

#define STATENAMES \
"OFF\000              ",\
"SELF TEST INIT\000   ",\
"WARMUP\000           ",\
"NORMAL OPERATION\000 ",\
"SHUT DOWN\000        ",\
"POST FAILURE\000     ",\
"EMERGENCY STOP\000   ",\
"MANUAL MODE          ",\
"DIAGNOSTICS MODE\000 ",\
"UNKNOWN\000          "

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
