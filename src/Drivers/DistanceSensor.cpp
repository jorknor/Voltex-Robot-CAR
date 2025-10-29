#include "DistanceSensor.h"

unsigned int getDistanceLeft() {
unsigned long leftDuration = pulseIn(leftEchoPin, HIGH, 10000);
unsigned int leftDistance_cm = leftDuration * 0.034 / 2;
return leftDistance_cm;
}
unsigned int getDistanceRight() {
  unsigned long rightDuration = pulseIn(rightEchoPin, HIGH, 10000);
unsigned int rightDistance_cm = rightDuration * 0.034 / 2;
return rightDistance_cm;
}
unsigned int getDistanceMiddle() {
  unsigned long middleDuration = pulseIn(middleEchoPin, HIGH, 10000);
unsigned int middleDistance_cm = middleDuration * 0.034 / 2;
return middleDistance_cm;
}
