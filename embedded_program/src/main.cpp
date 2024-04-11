#include "instruction.h"
#include "stepper_interface.h"
#include "task_queue.h"
#include "embedded_strategy.h"
#include "signals.h"
#include "Arduino.h"
#include "printing.h"
#include "oled_screen.h"


const unsigned int MAX_SEQUENCE_LENGTH = 200;


void processOutsideSignals()
{
    if (gotEmergencyStopSignal()) {
        Serial.println("emergencySignal");
        abortRunningTask();
        struct instruction wait = {'w', 2.0}; // Wait 2 seconds
        clearQueue();
        enqueueInstruction(wait);
    } else{
        processExternalSignals();
        processExternalInstructions();
        // Serial.println("processOutsideSignals");
        // if(Serial1.available()){
        //     digitalWrite(D9, HIGH);
        //     int message = Serial1.read();
            
        //     enqueueInstruction({'f', (float) message});
        //     enqueueInstruction({'w', 0.5});
        // }
    }
}


void scheduleNextInstruction()
{
    if (isQueueEmpty())
        inspectEnvironmentAndComputeNewStrategy();
    setMotorsSteps(dequeueInstruction());
}

const unsigned int DELTA_T = 50; // Each time the board will inspect the received signals
unsigned int t = 0;

void setup()
{
    initiateBoardVars();
    t = 0;
    Serial.begin(9600);
}

void loop()
{
    if (t++ == DELTA_T) {
        t = 0;
        processOutsideSignals();
    }
    if (isStepperFree()){
        scheduleNextInstruction();
    }
    oledBlink(57);
    executeOneMotorStep();
}