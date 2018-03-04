#include "Machine.h"
#include "Monitor.h"
#include "Clock.h"
#include "SafetyThird.h"

const char statenames[][STATENAMELEN]={STATENAMES};
Machine machine;



void setup() {
    machine.init();
}

// POST
void Machine::init(){ 
    monitor.init();
    inSafeStateToRun=false;
    setState(STATE_SELF_TEST_INIT);
    safety.init();
    if (state() == STATE_SELF_TEST_INIT) {
        inSafeStateToRun=true;
        setState(STATE_WARM_UP);
    }
    monitorTask.set(100L, TASK_FOREVER, &Monitor::run);
    machine.todolist.addTask(monitorTask);
    monitorTask.enable();
    machineTask.set(500L, TASK_FOREVER, &systemInCharge);
    machine.todolist.addTask(machineTask);
    machineTask.enable();
    monitor.registerAction(_TMR_, &TMR);
    monitor.registerAction(_TMS_, &TMS);
    monitor.registerAction(_TRM_, &TRM);

}

const char * Machine::stateName(int s) { return statenames[s]; };
void loop() {
    machine.todolist.execute();
}

void Machine::setState(int newState) {
    previousState=currentState;
    currentState=newState;
    monitor.update("STC","%.04d %s ==> %s",
                   newState,
                   stateName(previousState),
                   stateName(newState));
}
#define ENTERING (machine.previousState!=machine.currentState)
// this is the state machine.

static void systemInCharge(){
    static time_t lastUpdate;
    
    switch (machine.state()){
            
        case STATE_OFF:
        case STATE_SELF_TEST_INIT:break;
            
        case STATE_WARM_UP:
            if (ENTERING) {
                safety.redLightVal=RED_FULL_ON;//fix number foo
            } else {
                machine.setState(STATE_RUN_MODE);
            }
            break;
        case STATE_RUN_MODE:
            if (ENTERING) {
                 lastUpdate=clock.time();
            }
            time_t delta;
            delta=lastUpdate=(clock.time()-lastUpdate);
            if ((delta>0)
                && (machine.timerIsRunning)
                ){
                machine.timeRemaining -= (int) delta;
                if (machine.timeRemaining<0) {
                    machine.timeRemaining=0;
                }
                monitor.update("TRM","%d",machine.timeRemaining);
            }
            if ((machine.timeRemaining==0)
                && machine.timerIsRunning
                ){
                //pump.Release();
                machine.timerIsRunning=false;
                monitor.log("-- Job finished --");
                machine.timeRemaining=machine.timerSetting;
                monitor.update("TRM","%d",machine.timeRemaining);
                monitor.update("TMR","%d",machine.timerIsRunning);
            }
            break;
            
            
        case STATE_SHUTDOWN:
        case STATE_POST_FAILURE:
        case STATE_E_SHUTDOWN:
            if (ENTERING) {
                safety.redLightVal=255;//fix number foo
            } else {
                machine.setState(STATE_RUN_MODE);
            }
            break;
        case STATE_MANUAL_MODE:
        case STATE_DIAGNOSTICS_MODE:break;
        case STATE_UNKNOWN_STATE:break;
    }
    machine.previousState=machine.currentState;
    lastUpdate=clock.time();
}
/*----------------------------------------------------------------------
 *  Monitor Callbacks
 *--------------------------------------------------------------------*/

static void TMR (uint8_t kwIndex, uint8_t verb,char *args) {
    if (verb == '!') {
        bool wasRunning=machine.timerIsRunning;
        bool start=machine.timerIsRunning=atoi(args);
        if (start) {
            //
            monitor.log("-- Job (Re)Started --"); //if was notrunning?
        } else {
            if (wasRunning) {
                monitor.log("-- Job Paused --");
            }
        }
    }
    monitor.update("TMR","%d",machine.timerIsRunning);
}

static void TMS (uint8_t kwIndex, uint8_t verb,char *args) {
    
    if (verb == '!') {
        machine.timeRemaining=machine.timerSetting=atoi(args);
        monitor.update("TRM","%d",machine.timeRemaining);
    }
    monitor.update("TMS","%d",machine.timerSetting);
}

static void TRM (uint8_t kwIndex, uint8_t verb,char *args) {
    
    if (verb == '!') {
        machine.timeRemaining=atoi(args);
        monitor.update("TRM","%d",machine.timeRemaining);
    }
    monitor.update("TRM","%d",machine.timeRemaining);
}
