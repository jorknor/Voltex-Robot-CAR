#include "../MainStateMachine.h"
#include "../Drivers/MotorDriver.h"

void idleState(struct State* currState){
  setMotorState(HALTED);
// insert (remote control om state van de robot te veranderen) here
}
