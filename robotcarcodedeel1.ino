/*
  Complete Robotcar firmware (Arduino C++)
  - L298N motor driver
  - 3x IR lijnsensoren
  - HC-SR04 ultrasoon (voor afstand & slave-mode)
  - Bluetooth (Serial / HC-05) bediening
  - LED indicators bij links/rechts draaien
  - Display I2C (LiquidCrystal_I2C)
  - EEPROM: sla baseSpeed en mode op en laad bij opstart
  - Eenvoudige obstakel-omrij-logica (back-up + draai)
  - State machine: 0=STOP,1=LINE,2=SLAVE,3=REMOTE
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// ---------- PINS ----------
const int sensorLeft   = A0;
const int sensorCenter = A1;
const int sensorRight  = A2;

const int ENA = 3;
const int IN1 = 4;
const int IN2 = 5;

const int ENB = 6;
const int IN3 = 9;
const int IN4 = 10;

const int trigPin = 7;
const int echoPin = 8;

const int buzzer = 11;
const int ledLeft = 12;
const int ledRight = 13;

// ---------- DISPLAY ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- SETTINGS ----------
int threshold = 500;
int baseSpeed = 150;
int turnBoost = 80;
int safeDistance = 20;
int state = 0; // 0=STOP,1=LINE,2=SLAVE,3=REMOTE

const int EEPROM_ADDR_SPEED = 0;
const int EEPROM_ADDR_STATE = 1;

unsigned long lastCommandTime = 0;
const unsigned long remoteTimeout = 3000;

unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 500;

// ---------- FUNCTION DECLARATIONS ----------
void setLeftMotor(int speed);
void setRightMotor(int speed);
void forward(int leftSpeed, int rightSpeed);
void backward(int leftSpeed, int rightSpeed);
void stopMotors();
void blinkBuzzer(int times);
void handleCommand(char cmd);
void lineFollowStep();
bool attemptRecoverFromLostLine();
void slaveStep();
bool avoidObstacle();
int getDistance();
void updateDisplay(bool force);
void saveSettings();
void loadSettings();
void clearSettings();

// ---------- SETUP ----------
void setup() {
  pinMode(sensorLeft, INPUT);
  pinMode(sensorCenter, INPUT);
  pinMode(sensorRight, INPUT);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);
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
  if (Serial.available()) {
    char c = Serial.read();
    handleCommand(c);
    lastCommandTime = millis();
  }

  if (state == 3 && millis() - lastCommandTime > remoteTimeout) {
    stopMotors();
    state = 0;
    saveSettings();
  }

  switch (state) {
    case 0: break;
    case 1: lineFollowStep(); break;
    case 2: slaveStep(); break;
    case 3: break; // REMOTE handled in handleCommand
  }

  if (millis() - lastDisplayUpdate > displayInterval) {
    updateDisplay(false);
    lastDisplayUpdate = millis();
  }
}

// ---------- COMMAND HANDLER ----------
void handleCommand(char cmd) {
  if (cmd >= '0' && cmd <= '3') {
    state = cmd - '0';
    saveSettings();
    return;
  }

  if (cmd == 'F') { state = 3; forward(baseSpeed, baseSpeed); saveSettings(); return; }
  if (cmd == 'B') { state = 3; backward(baseSpeed, baseSpeed); saveSettings(); return; }
  if (cmd == 'L') { state = 3; forward(baseSpeed - turnBoost, baseSpeed + turnBoost); saveSettings(); return; }
  if (cmd == 'R') { state = 3; forward(baseSpeed + turnBoost, baseSpeed - turnBoost); saveSettings(); return; }
  if (cmd == 'S') { state = 0; stopMotors(); saveSettings(); return; }

  if (cmd == '+') { baseSpeed = min(baseSpeed + 20, 255); saveSettings(); return; }
  if (cmd == '-') { baseSpeed = max(baseSpeed - 20, 50); saveSettings(); return; }

  if (cmd == 'W') { saveSettings(); blinkBuzzer(2); return; }
  if (cmd == 'C') { clearSettings(); blinkBuzzer(2); return; }
}

// ---------- MOTOR CONTROL ----------
void setLeftMotor(int speed) {
  speed = constrain(speed, -255, 255);
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

// ---------- BUZZER ----------
void blinkBuzzer(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(buzzer, HIGH);
    delay(120);
    digitalWrite(buzzer, LOW);
    delay(80);
  }
}

// ---------- LINE FOLLOW ----------
void lineFollowStep() {
  int l = analogRead(sensorLeft);
  int c = analogRead(sensorCenter);
  int r = analogRead(sensorRight);

  bool L = l < threshold;
  bool C = c < threshold;
  bool R = r < threshold;

  digitalWrite(ledLeft, L && !C);
  digitalWrite(ledRight, R && !C);

  if (C && !L && !R) forward(baseSpeed, baseSpeed);
  else if (L && !C) forward(constrain(baseSpeed - turnBoost, 0, 255),
                             constrain(baseSpeed + turnBoost, 0, 255));
  else if (R && !C) forward(constrain(baseSpeed + turnBoost, 0, 255),
                             constrain(baseSpeed - turnBoost, 0, 255));
  else if (L && C) forward(baseSpeed - 30, baseSpeed + 30);
  else if (R && C) forward(baseSpeed + 30, baseSpeed - 30);
  else {
    stopMotors();
    blinkBuzzer(1);
    if (!attemptRecoverFromLostLine()) {
      state = 0;
      stopMotors();
      blinkBuzzer(2);
    }
  }
}

bool attemptRecoverFromLostLine() {
  backward(120, 120); delay(300); stopMotors(); delay(100);
  forward(100, -100); delay(300); stopMotors(); delay(100);
  if (analogRead(sensorCenter) < threshold) return true;
  forward(-100, 100); delay(600); stopMotors(); delay(100);
  if (analogRead(sensorCenter) < threshold) return true;
  return false;
}

// ---------- SLAVE ----------
void slaveStep() {
  int dist = getDistance();
  if (dist == 0) { stopMotors(); blinkBuzzer(2); state = 0; return; }

  if (dist > safeDistance + 5) forward(baseSpeed, baseSpeed);
  else if (dist >= safeDistance - 2) forward(baseSpeed - 30, baseSpeed - 30);
  else {
    stopMotors(); delay(50);
    if (!avoidObstacle()) { stopMotors(); blinkBuzzer(3); state = 0; }
  }
}

bool avoidObstacle() {
  stopMotors(); delay(100);
  backward(140, 140); delay(350); stopMotors(); delay(100);
  forward(140, -140); delay(450); stopMotors(); delay(100);
  if (getDistance() > safeDistance + 5) { forward(baseSpeed, baseSpeed); return true; }
  forward(-140, 140); delay(900); stopMotors(); delay(100);
  if (getDistance() > safeDistance + 5) { forward(baseSpeed, baseSpeed); return true; }
  stopMotors(); return false;
}

// ---------- ULTRASOUND ----------
int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) return 0;
  return duration * 0.034 / 2;
}

// ---------- DISPLAY ----------
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
    case 1: lcd.print("Mode: LINE"); break;
    case 2: lcd.print("Mode: SLAVE"); break;
    case 3: lcd.print("Mode: REMOTE"); break;
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
  if (m <= 3) state = m;
}

void clearSettings() {
  baseSpeed = 150;
  state = 0;
  EEPROM.update(EEPROM_ADDR_SPEED, baseSpeed);
  EEPROM.update(EEPROM_ADDR_STATE, state);
}
