// === LINESENSOR TEST ===
// Werkt op ATmega328P Xplained Mini of Arduino Uno
#include "src/Drivers/LineSenor.h"
  
const int sensorLeft   = A0;
const int sensorCenter = A1;
const int sensorRight  = A2;

int threshold = 500;  // grenswaarde tussen zwart/wit — pas dit aan na test

void setup() {
  Serial.begin(9600);
  pinMode(sensorLeft, INPUT);
  pinMode(sensorCenter, INPUT);
  pinMode(sensorRight, INPUT);

  Serial.println("== Line Sensor Test Start ==");
  Serial.println("Plaats de sensor boven zwart of wit oppervlak...");
}

void loop() {
  
  Serial.println(lineSensors[1]);

  delay(300);
}

