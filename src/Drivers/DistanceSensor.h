#pragma once
#define trigPin  2
#define leftEchoPin  3
#define rightEchoPin  4
#define middleEchoPin  5
extern unsigned int leftDistance_cm;
extern unsigned int middleDistance_cm;
extern unsigned int rightDistance_cm;
unsigned int getDistanceLeft();
unsigned int getDistanceMiddle();
unsigned int getDistanceRight();
