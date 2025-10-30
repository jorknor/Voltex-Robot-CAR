#pragma once
#include <Arduino.h>
#include "../Drivers/DistanceSensor.h"
#include "../MainStateMachine.h"
#include "../Drivers/MotorDriver.h"
#include "../MainStateMachine.h"

void autonomousState(struct State* currState);

