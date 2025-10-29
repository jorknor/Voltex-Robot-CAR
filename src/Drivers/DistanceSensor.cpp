#include "DistanceSensor.h"

unsigned int leftDistance_cm;
unsigned int rightDistance_cm;
unsigned int middleDistance_cm;

void distanceUpdate(){
 unsigned long leftDuration = pulseIn(leftEchoPin, HIGH, 10000);
  leftDistance_cm = leftDuration * 0.034 / 2;
  unsigned long rightDuration = pulseIn(rightEchoPin, HIGH, 10000);
  rightDistance_cm = rightDuration * 0.034 / 2;
  unsigned long middleDuration = pulseIn(middleEchoPin, HIGH, 10000);
  middleDistance_cm = middleDuration * 0.034 / 2;
}
