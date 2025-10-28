#include "../Drivers/DistanceSensor.h"
#include "../Drivers/Linesensor.h"
#include "../MainStateMachine.h"
#include "../Drivers/MotorDriver.h"

void slaveState(struct State* currState){
  if (HLS_PIN == MLS_PIN == MRS_PIN == HRS_PIN){
    setMotorState(FORWARD);
  }
  else if(MLS_PIN == TRUE){
    setMotorState(LEFT);
  }
  else if(HLS_PIN == TRUE){
    setMotorState(HARD_LEFT);
  }
  else if(MRS_PIN == TRUE){
    setMotorState(RIGHT);
  }
  else if(HRS_PIN == TRUE){
    setMotorState(HARD_RIGHT);
  }
  else{
      if (currState->currentEvent == Slave) {
            currentState->id = Idle;
      }
   }
}

