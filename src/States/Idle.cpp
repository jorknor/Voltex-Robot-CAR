#include "../MainStateMachine.h"
#include "../Drivers/MotorDriver.h"
#include "Idle.h"
void idleState(struct State* currState){
  setMotorState(HALTED);
}

