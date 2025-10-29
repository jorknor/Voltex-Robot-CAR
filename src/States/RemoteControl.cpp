#include "RemoteControl.h"

void remoteControlState(struct State* currState){
  if (currState->currentEvent == InputComplete){
    if (strncmp(inputBuffer[0], "w", 1) == 0){
      setMotorState(FORWARDS);
    } else if (strncmp(inputBuffer[0], "a", 1) == 0) {
      setMotorState(LEFT);
    } else if (strncmp(inputBuffer[0], "d", 1) == 0) {
      setMotorState(RIGHT);
    } else if (strncmp(inputBuffer[0], "s", 1) == 0) {
      setMotorState(BACKWARDS);
    } else {
      setMotorState(HALTED);
    }
    flushArguments();
  }
}


