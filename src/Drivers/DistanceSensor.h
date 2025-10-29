#pragma once
#include <Arduino.h>
#define trigPin  8
#define leftEchoPin  11
#define rightEchoPin  9
#define middleEchoPin 10

extern unsigned int leftDistance_cm;
extern unsigned int middleDistance_cm;
extern unsigned int rightDistance_cm;
void distanceUpdate();
