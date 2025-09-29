/*
  Verbeterde Robotcar firmware (Arduino C++)
  Aanpassingen:
  - Fail-safe lijnzoeker
  - Motor PWM balancering
  - Extra Bluetooth commands (Q/E/U/D)
  - Non-blocking buzzer
  - EEPROM optimalisatie (alleen bij wijziging)
  - Ultrasoon minder frequent meten
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// ---------- SENSOR PINS ----------
const int sensorFarLeft = A3;
const int sensorLeft    = A0;
const int sensorCenter  = A1;
const int sensorRight   = A2;

// ---------- MOTOR DRIVER (L298N) ----------
const int ENA = 3;
const int IN1 = 4;
const int IN2 = 5;

const int ENB = 6;
const int IN3 = 9;
const int IN4 = 10;

// ---------- ULTRASOON ----------
const int trigPin = 7;
const int echoPin = 8;

// ---------- OUTPUTS ----------
const int buzzer = 11;
const int ledLeft = 12;
const int ledRight = 13;

// ---------- DISPLAY ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- INSTELLINGEN ----------
int threshold = 500;
int baseSpeed = 150;
int turnBoost = 80;
int safeDistance = 20;
int state = 0;

// Motor balans
int leftOffset = -10;
int rightOffset = 0;

// ---------- EEPROM ADRESSEN ----------
const int EEPROM_ADDR_SPEED = 0;
const int EEPROM_ADDR_STATE = 1;

// ---------- TIMING ----------
unsigned long lastCommandTime = 0;
const unsigned long remoteTimeout = 3000;

unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 500;

unsigned long lastDistMeasure = 0;
const unsigned long distInterval = 200; // max 5x per seconde meten
int cachedDist = 0;

// Buzzer non-blocking
unsigned long buzzerUntil = 0;

// ---------- FUNCTIES ----------
void setLeftMotor(int speed);
void setRightMotor(int speed);
void forward(int leftSpeed, int rightSpeed);
void backward(int leftSpeed, int rightSpeed);
void stopMotors();
void beepAsync(int times);
void handleCommand(char cmd);
void lineFollowStep();
int getDistance();
void updateDisplay(bool force);
void saveSettings();
void loadSettings();
void clearSettings();

// ---------- SETUP ----------
void setup() {
  pinMode(sensorFarLeft, INPUT);
  pinMode(sensorLeft, INPUT);
  pinMode(sensorCenter, INPUT);
  pinMode(sensorRight, INPUT);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  loadSettings();
  stopMotors();
  updateDisplay(true);
}

// ---------- LOOP ----------
void loop() {
  // Bluetooth
  if (Serial.available()) {
    char c = Serial.read();
    handleCommand(c);
    lastCommandTime = millis();
  }

  // Timeout remote
  if (state == 2 && millis() - lastCommandTime > remoteTimeout) {
    stopMotors();
    state = 0;
    saveSettings();
  }

  switch (state) {
    case 0: break;
    case 1: lineFollowStep(); break;
    case 2: break;
  }

  // LCD update
  if (millis() - lastDisplayUpdate > displayInterval) {
    updateDisplay(false);
    lastDisplayUpdate = millis();
  }

  // Non-blocking buzzer uitzetten
  if (millis() > buzzerUntil) digitalWrite(buzzer, LOW);
}

// ---------- BLUETOOTH COMMANDOS ----------
void handleCommand(char cmd) {
  int prevState = state;
  int prevSpeed = baseSpeed;

  if (cmd >= '0' && cmd <= '2') { state = cmd - '0'; }
  else if (cmd == 'F') { state = 2; forward(baseSpeed, baseSpeed); }
  else if (cmd == 'B') { state = 2; backward(baseSpeed, baseSpeed); }
  else if (cmd == 'L') { state = 2; forward(baseSpeed - turnBoost, baseSpeed + turnBoost); }
  else if (cmd == 'R') { state = 2; forward(baseSpeed + turnBoost, baseSpeed - turnBoost); }
  else if (cmd == 'Q') { state = 2; forward(baseSpeed - 30, baseSpeed + 30); } // zachte links
  else if (cmd == 'E') { state = 2; forward(baseSpeed + 30, baseSpeed - 30); } // zachte rechts
  else if (cmd == 'S') { state = 0; stopMotors(); }
  else if (cmd == '+') { baseSpeed = min(baseSpeed + 20, 255); }
  else if (cmd == '-') { baseSpeed = max(baseSpeed - 20, 50); }
  else if (cmd == 'U') { baseSpeed = 200; } // snel
  else if (cmd == 'D') { baseSpeed = 100; } // langzaam
  else if (cmd == 'C') { clearSettings(); beepAsync(2); }

  // EEPROM alleen opslaan bij wijziging
  if (prevState != state || prevSpeed != baseSpeed) {
    saveSettings();
  }
}

// ---------- MOTORFUNCTIES ----------
void setLeftMotor(int speed) {
  speed = constrain(speed + leftOffset, -255, 255);
  if (speed >= 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    analogWrite(ENA, -speed);
  }
}

void setRightMotor(int speed) {
  speed = constrain(speed + rightOffset, -255, 255);
  if (speed >= 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
  } else {
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    analogWrite(ENB, -speed);
  }
}

void forward(int leftSpeed, int rightSpeed) { setLeftMotor(leftSpeed); setRightMotor(rightSpeed); }
void backward(int leftSpeed, int rightSpeed) { setLeftMotor(-leftSpeed); setRightMotor(-rightSpeed); }
void stopMotors() { analogWrite(ENA, 0); analogWrite(ENB, 0); }

// ---------- BUZZER ----------
void beepAsync(int times) {
  buzzerUntil = millis() + (times * 200);
  digitalWrite(buzzer, HIGH);
}

// ---------- LIJN VOLGEN + FAIL-SAFE ----------
void lineFollowStep() {
  int fl = analogRead(sensorFarLeft);
  int l  = analogRead(sensorLeft);
  int c  = analogRead(sensorCenter);
  int r  = analogRead(sensorRight);

  bool FL = fl < threshold;
  bool L  = l < threshold;
  bool C  = c < threshold;
  bool R  = r < threshold;

  int dist = getDistance();
  int speedAdj = baseSpeed;
  if (dist > 0 && dist < safeDistance) speedAdj = 0;
  else if (dist > 0 && dist < safeDistance + 5) speedAdj = baseSpeed / 2;

  if (C && !L && !R && !FL) forward(speedAdj, speedAdj);
  else if (L && !C && !R) forward(speedAdj - turnBoost, speedAdj + turnBoost);
  else if (R && !C && !L) forward(speedAdj + turnBoost, speedAdj - turnBoost);
  else if (FL) forward(speedAdj - 100, speedAdj + 100);
  else if (L && C) forward(speedAdj - 40, speedAdj + 40);
  else if (R && C) forward(speedAdj + 40, speedAdj - 40);
  else {
    // FAIL-SAFE: zoek lijn terug door langzaam draaien
    forward(-80, 80);
  }
}

// ---------- ULTRASOON ----------
int getDistance() {
  if (millis() - lastDistMeasure < distInterval) return cachedDist;
  lastDistMeasure = millis();

  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) return cachedDist;
  cachedDist = duration * 0.034 / 2;
  return cachedDist;
}

// ---------- LCD DISPLAY ----------
void updateDisplay(bool force) {
  static int lastState = -1;
  static int lastSpeed = -1;
  static int lastDist = -1;
  int dist = getDistance();

  if (!force && lastState == state && lastSpeed == baseSpeed && lastDist == dist) return;
  lastState = state; lastSpeed = baseSpeed; lastDist = dist;

  lcd.clear();
  lcd.setCursor(0, 0);
  switch (state) {
    case 0: lcd.print("Mode: STOP"); break;
    case 1: lcd.print("Mode: LINE+SLAVE"); break;
    case 2: lcd.print("Mode: REMOTE"); break;
    default: lcd.print("Mode: ?"); break;
  }

  lcd.setCursor(0, 1);
  lcd.print("Spd:" + String(baseSpeed) + " D:" + String(dist) + "cm");
}

// ---------- EEPROM ----------
void saveSettings() {
  EEPROM.update(EEPROM_ADDR_SPEED, (uint8_t)baseSpeed);
  EEPROM.update(EEPROM_ADDR_STATE, (uint8_t)state);
}

void loadSettings() {
  int s = EEPROM.read(EEPROM_ADDR_SPEED);
  int m = EEPROM.read(EEPROM_ADDR_STATE);
  if (s >= 50 && s <= 255) baseSpeed = s;
  if (m <= 2) state = m;
}
/////
void clearSettings() {
  baseSpeed = 150;
  state = 0;
  EEPROM.update(EEPROM_ADDR_SPEED, baseSpeed);
  EEPROM.update(EEPROM_ADDR_STATE, state);
}
