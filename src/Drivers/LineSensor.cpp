#include "LineSensor.h"
int lineSensors [4] ={0};
void lineSensorInit(){
  pinmode (HLS_PIN, input);
  pinmode (MLS_PIN, input);
  pinmode (MRS_PIN, input);
  pinmode (HRS_PIN, input);
}
void lineSensorUpdate(){
  lineSensors[0] = digitalRead(HLS_PIN);
  lineSensors[1] = digitalRead(MLS_PIN);
  lineSensors[2] = digitalRead(MRS_PIN);
  lineSensors[3] = digitalRead(HRS_PIN);
}






