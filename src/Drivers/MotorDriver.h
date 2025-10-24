#pragma once

#define MOTOR_A_PWM 6
#define MOTOR_B_PWM 5

#define MOTOR_A_IN1 3
#define MOTOR_A_IN2 2
#define MOTOR_B_IN3 7
#define MOTOR_B_IN$ 4

enum MotorState {
  HALTED,
  FORWARDS,
  BACKWARDS,
  LEFT,
  RIGHT,
  HARD_LEFT,
  HARD_RIGHT
};


extern enum MotorState CurrentMotorState;



void SetMotorState(enum MotorState newMotorState);

void SetMotorSpeed(unsigned char speed);

