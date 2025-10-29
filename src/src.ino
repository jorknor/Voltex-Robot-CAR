/*
  Final Robotcar firmware (Arduino C++)
  Functies:
   - L298N motor driver (PWM op ENA/ENB)
   - 4x IR lijnsensoren (A0..A3)
   - HC-SR04 ultrasoon (veiligheid / volgmodus)
   - Bluetooth (HC-05/HС-06) via hardware Serial (pins 0/1)
   - LED indicators
   - I2C LCD (LiquidCrystal_I2C)
   - EEPROM: onthoudt baseSpeed en mode
   - State machine:
       0 = STOP
       1 = LINE+SLAVE (lijn volgen + afstand houden)
       2 = REMOTE (besturen via bluetooth)
   - Non-blocking buzzer, fail-safe lijnzoeker, EEPROM schrijfoptimalisatie
*/
#include "MainStateMachine.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "Drivers/LineSensor.h"


void setup() {
    lineSensorInit();
    Serial.begin (9600);
  

  
}

void loop() {
    Serial.print(lineSensors[0]);
    lineSensorUpdate();
    delay(300);
}

// ---------- EEPROM ----------
/*
void saveSettings() {
  EEPROM.update(EEPROM_ADDR_SPEED, (uint8_t)baseSpeed);
  EEPROM.update(EEPROM_ADDR_STATE, (uint8_t)state);
}

void loadSettings() {
  int s = EEPROM.read(EEPROM_ADDR_SPEED);
  int m = EEPROM.read(EEPROM_ADDR_STATE);
  if (s >= 50 && s <= 255) baseSpeed = s;
  if (m >= 0 && m <= 2) state = m;
}

void clearSettings() {
  baseSpeed = 150;
  state = 0;
  EEPROM.update(EEPROM_ADDR_SPEED, baseSpeed);
  EEPROM.update(EEPROM_ADDR_STATE, state);
}
*/



