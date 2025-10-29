#include "SerialDriver.h"

bool serialInputComplete = false;
int serialInputSize = 0;
char serialBuffer[SERIAL_BUFFER_SIZE] = {0};
void serialUpdate(){
while (Serial.available()){
  if (serialInputComplete == false){
    if (serialInputSize < SERIAL_BUFFER_SIZE){
        serialbuffer[serialInputSize] = Serial.read();
        serialInputSize++;
      }
    }
  }
}
void serialInit(){
  Serial.begin(9600);
}
