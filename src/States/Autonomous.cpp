#include "../Drivers/DistanceSensor.h"
#include "../MainStateMachine.h"
#include "../Drivers/MotorDriver.h"
#include "Autonomous.h"
void autonomousState(struct State* currState){
  if (middleDistance_cm <30){
    setMotorState(HARD_LEFT);
  }
  else if (leftDistance_cm <40){
    setMotorState(RIGHT);
  }
   else if (rightDistance_cm <40){
    setMotorState(LEFT);
  }
}  

