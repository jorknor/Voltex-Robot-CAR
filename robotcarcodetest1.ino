
// deze code zou de test code kunnen Zijn van de volledige code

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

  pinMode(sensorLeft, INPUT);
  pinMode(sensorMidLeft, INPUT);
  pinMode(sensorMidRight, INPUT);
  pinMode(sensorRight, INPUT);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  lcd.init();
  lcd.backlight();

  Serial.println("=== ROBOTCAR TEST MODE (4 sensors + slave) ===");
}

void loop() {
  // ----- LINE SENSOR TEST -----
  int l  = analogRead(sensorLeft);
  int ml = analogRead(sensorMidLeft);
  int mr = analogRead(sensorMidRight);
  int r  = analogRead(sensorRight);

  Serial.print("L: "); Serial.print(l);
  Serial.print(" ML: "); Serial.print(ml);
  Serial.print(" MR: "); Serial.print(mr);
  Serial.print(" R: "); Serial.println(r);

  // ----- ULTRASOON SENSOR -----
  int dist = getDistance();
  Serial.print("Ultrason distance: "); Serial.print(dist); Serial.println(" cm");

  // ----- SLAVE + LINE FOLLOW SIMULATIE -----
  if (dist > 15 || dist == 0) {  
    // veilig -> lijn volgen
    if (ml > 500 && mr > 500) {
      forward(150, 150);  // midden -> rechtdoor
    } else if (l > 500 || ml > 500) {
      forward(100, 150);  // links -> corrigeer links
      digitalWrite(ledLeft, HIGH);
    } else if (r > 500 || mr > 500) {
      forward(150, 100);  // rechts -> corrigeer rechts
      digitalWrite(ledRight, HIGH);
    } else {
      stopMotors(); // geen lijn -> stoppen
      blinkBuzzer(1);
    }
  } else {
    // te dichtbij -> stoppen
    stopMotors();
    blinkBuzzer(2);
    Serial.println("!!! STOP: obstakel !!!");
  }

  // ----- LCD TEST -----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Line/Slave TEST");
  lcd.setCursor(0, 1);
  lcd.print("Dist:" + String(dist) + "cm");

  delay(500); // korte pauze
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

// ---------- ULTRASOON SENSOR ----------
int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) return 0;
  return duration * 0.034 / 2;
}
