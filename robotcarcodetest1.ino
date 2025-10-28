
// deze code zou de test code kunnen Zijn van de volledige code

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Drivers/MoterDriver.h"
// ---------- PINS ----------
// Line sensors (4x)
const int sensorLeft     = A0;
const int sensorMidLeft  = A1;
const int sensorMidRight = A2;
const int sensorRight    = A3;

// Motor A (L298N)
const int ENA = 3;
const int IN1 = 4;
const int IN2 = 5;

// Motor B (L298N)
const int ENB = 6;
const int IN3 = 9;
const int IN4 = 10;

// Ultrasonic
const int trigPin = 7;
const int echoPin = 8;

// Buzzer + LEDs
const int buzzer   = 11;
const int ledLeft  = 12;
const int ledRight = 13;

// ---------- DISPLAY ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- UTILS ----------
void setLeftMotor(int speed);
void setRightMotor(int speed);
void forward(int leftSpeed, int rightSpeed);
void backward(int leftSpeed, int rightSpeed);
void stopMotors();
void blinkBuzzer(int times);
int getDistance();

void setup() {
  Serial.begin(9600);
  setMoterState(FOWARDS);
  setMoterSpeed 255
} 
  
void loop() {
  // ----- LINE SENSOR TEST -----
  
 
  // ----- LCD TEST -----
  
}

// ---------- MOTOR FUNCTIONS ----------
void setLeftMotor(int speed) {
  speed = constrain(speed, -256, 255);
  if (speed >= 0) { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); analogWrite(ENA, speed); }
  else { digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); analogWrite(ENA, -speed); }
}

void setRightMotor(int speed) {
  speed = constrain(speed, -256, 255);
  if (speed >= 0) { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); analogWrite(ENB, speed); }
  else { digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); analogWrite(ENB, -speed); }
}

void forward(int leftSpeed, int rightSpeed) { setLeftMotor(leftSpeed); setRightMotor(rightSpeed); }
void backward(int leftSpeed, int rightSpeed) { setLeftMotor(-leftSpeed); setRightMotor(-rightSpeed); }
void stopMotors() { analogWrite(ENA, 0); analogWrite(ENB, 0); digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); }

// ---------- BUZZER ----------
void blinkBuzzer(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(buzzer, HIGH); delay(120);
    digitalWrite(buzzer, LOW); delay(80);
  }
}


}
