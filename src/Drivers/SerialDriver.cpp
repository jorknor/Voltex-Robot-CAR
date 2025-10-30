#include "SerialDriver.h"

bool serialInputComplete = false;
int serialInputSize = 0;
char serialBuffer[SERIAL_BUFFER_SIZE] = {0};

void serialUpdate(){
  while (Serial.available()){
    if (serialInputSize < SERIAL_BUFFER_SIZE) {
      char inChar = Serial.read();

      if (inChar == '\n') {
        serialInputComplete = true;
      }

      if (serialInputComplete == false) {
        serialBuffer[serialInputSize] = inChar;
        serialInputSize++;
      }
    }
  }
}

void serialInit(){
  Serial.begin(9600);
}
