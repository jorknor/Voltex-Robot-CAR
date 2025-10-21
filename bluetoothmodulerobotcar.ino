/*
  Robotcar Bluetooth Control (Keyes BT 4.0)
  Eenvoudige besturing via app-knoppen
  A = vooruit
  B = achteruit
  C = links
  D = rechts
  E = stop
*/

const int ENA = 3;
const int IN1 = 4;
const int IN2 = 5;

const int ENB = 6;
const int IN3 = 9;
const int IN4 = 10;

int baseSpeed = 150;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
  Serial.println("Bluetooth Robot Ready");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    Serial.print("Ontvangen: ");
    Serial.println(cmd);

    switch (cmd) {
      case 'A': forward(); break;
      case 'B': backward(); break;
      case 'C': left(); break;
      case 'D': right(); break;
      case 'E': stopMotors(); break;
      default: break;
    }
  }
}

// ====== Motorfuncties ======

void forward() {
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Vooruit");
}

void backward() {
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Achteruit");
}

void left() {
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Links");
}

void right() {
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Rechts");
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stop");
}