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

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// ---------- SENSOR PINS (analog) ----------
const int sensorFarLeft = A3;
const int sensorLeft    = A2;
const int sensorCenter  = A1;
const int sensorRight   = A0;

// ---------- MOTOR DRIVER (L298N) ----------
// Let op: dit mapping gebruikt standaard Arduino UNO-compatibele pinnen (ATmega328P)
const int ENA = 3;   // PWM (left speed)
const int IN1 = 4;   // left dir
const int IN2 = 5;

const int ENB = 6;   // PWM (right speed)
const int IN3 = 9;   // right dir
const int IN4 = 10;

// ---------- ULTRASOON (3x HC-SR04) ----------
const int trigFront = 7;
const int echoFront = 8;

const int trigLeft  = A4;   // of een andere vrije digitale pin
const int echoLeft  = A5;

const int trigRight = 2;
const int echoRight = 9;

// er zijn drie ultrasonische sensors, dus max 6 pinnen
// ---------- OUTPUTS ----------
const int buzzer = 11;
const int ledLeft = 12;
const int ledRight = 13;

// ---------- DISPLAY (I2C) ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- INSTELLINGEN ----------
int threshold = 500;       // IR threshold (calibreer)
int baseSpeed = 150;       // 0-255
int turnBoost = 80;
int safeDistance = 20;     // cm
int state = 0;             // 0=STOP,1=LINE+SLAVE,2=REMOTE

// Motor balans (kalibratie)
int leftOffset = -10;
int rightOffset = 0;

// ---------- EEPROM ADDRESSES ----------
const int EEPROM_ADDR_SPEED = 0;
const int EEPROM_ADDR_STATE = 1;

// ---------- TIMING ----------
unsigned long lastCommandTime = 0;
const unsigned long remoteTimeout = 3000;
unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 500;
unsigned long lastDistMeasure = 0;
const unsigned long distInterval = 200; // max 5x/sec
int cachedDist = 0;

// buzzer non-blocking
unsigned long buzzerUntil = 0;

// Forward decls
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

void setup() {
  // sensors
  pinMode(sensorFarLeft, INPUT);
  pinMode(sensorLeft, INPUT);
  pinMode(sensorCenter, INPUT);
  pinMode(sensorRight, INPUT);

  // motor driver pins
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  // ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // outputs
  pinMode(buzzer, OUTPUT);
  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  // Serial (HC-05 -> hardware Serial: pins 0/1)
  Serial.begin(9600);

  // LCD
  lcd.init();
  lcd.backlight();

  // load
  loadSettings();
  stopMotors();
  updateDisplay(true);
}

void loop() {
  // Bluetooth commands (via hardware Serial)
  if (Serial.available()) {
    char c = Serial.read();
    handleCommand(c);
    lastCommandTime = millis();
  }

  // Timeout when in REMOTE mode (safety)
  if (state == 2 && millis() - lastCommandTime > remoteTimeout) {
    stopMotors();
    state = 0;
    saveSettings();
    beepAsync(1);
  }

  // Behavior per state
  switch (state) {
    case 0: /* STOP */ break;
    case 1: lineFollowStep(); break;
    case 2: /* REMOTE: handled by commands */ break;
  }

  // display update
  if (millis() - lastDisplayUpdate > displayInterval) {
    updateDisplay(false);
    lastDisplayUpdate = millis();
  }

  // buzzer turn off
  if (millis() > buzzerUntil) digitalWrite(buzzer, LOW);
}

// ---------- BLUETOOTH COMMANDS ----------
void handleCommand(char cmd) {
  int prevState = state;
  int prevSpeed = baseSpeed;

  if (cmd >= '0' && cmd <= '2') { state = cmd - '0'; }
  else if (cmd == 'F') { state = 2; forward(baseSpeed, baseSpeed); }
  else if (cmd == 'B') { state = 2; backward(baseSpeed, baseSpeed); }
  else if (cmd == 'L') { state = 2; forward(baseSpeed - turnBoost, baseSpeed + turnBoost); }
  else if (cmd == 'R') { state = 2; forward(baseSpeed + turnBoost, baseSpeed - turnBoost); }
  else if (cmd == 'Q') { state = 2; forward(baseSpeed - 30, baseSpeed + 30); } // soft left
  else if (cmd == 'E') { state = 2; forward(baseSpeed + 30, baseSpeed - 30); } // soft right
  else if (cmd == 'S') { state = 0; stopMotors(); }
  else if (cmd == '+') { baseSpeed = min(baseSpeed + 20, 255); }
  else if (cmd == '-') { baseSpeed = max(baseSpeed - 20, 50); }
  else if (cmd == 'U') { baseSpeed = 200; } // preset fast
  else if (cmd == 'D') { baseSpeed = 100; } // preset slow
  else if (cmd == 'W') { saveSettings(); beepAsync(2); } // write
  else if (cmd == 'C') { clearSettings(); beepAsync(2); } // clear defaults

  // save settings only if changed
  if (prevState != state || prevSpeed != baseSpeed) {
    saveSettings();
  }
}

// ---------- MOTOR FUNCTIONS ----------
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
void stopMotors() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ---------- BUZZER ----------
void beepAsync(int times) {
  buzzerUntil = millis() + (times * 200);
  digitalWrite(buzzer, HIGH);
}

// ---------- LINE FOLLOW + FAIL-SAFE ----------
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

  // logic
  if (C && !L && !R && !FL) forward(speedAdj, speedAdj);
  else if (L && !C && !R) forward(speedAdj - turnBoost, speedAdj + turnBoost);
  else if (R && !C && !L) forward(speedAdj + turnBoost, speedAdj - turnBoost);
  else if (FL) forward(speedAdj - 100, speedAdj + 100);
  else if (L && C) forward(speedAdj - 40, speedAdj + 40);
  else if (R && C) forward(speedAdj + 40, speedAdj - 40);
  else {
    // fail-safe: zoek lijn terug door langzaam draaien
    forward(-80, 80);
    digitalWrite(ledLeft, HIGH); digitalWrite(ledRight, LOW);
  }
}

// ---------- ULTRASONIC ----------
int getDistance() {
  if (millis() - lastDistMeasure < distInterval) return cachedDist;
  lastDistMeasure = millis();

  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) return cachedDist;
  cachedDist = (int)(duration * 0.034 / 2);
  return cachedDist;
}

// ---------- LCD ----------
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
  if (m >= 0 && m <= 2) state = m;
}

void clearSettings() {
  baseSpeed = 150;
  state = 0;
  EEPROM.update(EEPROM_ADDR_SPEED, baseSpeed);
  EEPROM.update(EEPROM_ADDR_STATE, state);
}
