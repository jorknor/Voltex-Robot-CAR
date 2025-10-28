
// deze code zou de test code kunnen Zijn van de volledige code

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Drivers/MoterDriver.h"
// ---------- PINS ----------

// Motor A (L298N)
const int ENA = 3;
const int IN1 = 4;
const int IN2 = 5;


const int ENB = 6;
const int IN3 = 9;
const int IN4 = 10;

// Ultrasonic

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

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.print("Motor Test Start");

  Serial.println("== Motor Test Start ==");

  // Test vooruit
  forward(200, 200);
  lcd.setCursor(0,1);
  lcd.print("Vooruit");
  delay(2000);

  // Test achteruit
  backward(200, 200);
  lcd.setCursor(0,1);
  lcd.print("Achteruit ");
  delay(2000);

  // Test links/rechts
  left();
  lcd.setCursor(0,1);
  lcd.print("Links     ");
  delay(2000);

  right();
  lcd.setCursor(0,1);
  lcd.print("Rechts    ");
  delay(2000);

  stopMotors();
  lcd.setCursor(0,1);
  lcd.print("Stop      ");
  Serial.println("== Motor Test Klaar ==");
}


// ---------- MOTOR FUNCTIONS ----------
void setLeftMotor(int speed) {
  speed = constrain(speed, -256, 255);
  if (speed >= 0) { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); analogWrite(ENA, speed); }
  else { digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); analogWrite(ENA, -speed); }
}

void setRightMotor(int speed) {
  speed = constrain(speed, -255, 255);
  if (speed >= 0) { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); analogWrite(ENB, speed); }
  else { digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); analogWrite(ENB, -speed); }
}

void forward(int leftSpeed, int rightSpeed) { setLeftMotor(leftSpeed); setRightMotor(rightSpeed); }
void backward(int leftSpeed, int rightSpeed) { setLeftMotor(-leftSpeed); setRightMotor(-rightSpeed); }
void stopMotors() { analogWrite(ENA, 0); analogWrite(ENB, 0); digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); }






