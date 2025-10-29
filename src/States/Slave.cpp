#include "../Drivers/DistanceSensor.h"
#include "../Drivers/Linesensor.h"
#include "../MainStateMachine.h"
#include "../Drivers/MotorDriver.h"

void slaveState(struct State* currState){
  if (lineSensors[0] && lineSensors[1] && lineSensors[2] && lineSensors[3]){
    setMotorState(FORWARD);
  }
  else if(lineSensors[1] == TRUE){
    setMotorState(LEFT);
  }
  else if(lineSensors[0] == TRUE){
    setMotorState(HARD_LEFT);
  }
  else if(lineSensors[2] == TRUE){
    setMotorState(RIGHT);
  }
  else if(lineSensors[3] == TRUE){
    setMotorState(HARD_RIGHT);
  }
  else{
      if (currState->currentEvent == Slave) {
            currentState->id = Idle;
      }
   }
  if (middleDistance_cm <20){
    changeMotorSpeed(-10);
  }
  else if (middleDistance_cm >40){
    changeMotorSpeed(10);
  }
}



