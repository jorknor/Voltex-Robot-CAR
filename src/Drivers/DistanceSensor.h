#pragma once
#include <Arduino.h>
#DEFINE trigPin  2
#DEFINE leftEchoPin  3
#DEFINE rightEchoPin  4
#DEFINE middleEchoPin  5
extern unsigned int leftDistance_cm;
extern unsigned int middleDistance_cm;
extern unsigned int rightDistance_cm;
unsigned int getDistanceLeft();
unsigned int getDistanceMiddle();
unsigned int getDistanceRight();
