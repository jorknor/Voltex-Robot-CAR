// === LINESENSOR TEST ===
// Werkt op ATmega328P Xplained Mini of Arduino Uno

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
  int leftVal = analogRead(sensorLeft);
  int midVal  = analogRead(sensorCenter);
  int rightVal = analogRead(sensorRight);

  // Toon rauwe waarden (0–1023)
  Serial.print("L: "); Serial.print(leftVal);
  Serial.print("  C: "); Serial.print(midVal);
  Serial.print("  R: "); Serial.print(rightVal);

  // Zwart/wit interpretatie
  Serial.print("  => ");
  Serial.print(leftVal < threshold ? "L=ZWART " : "L=WIT ");
  Serial.print(midVal < threshold ? "C=ZWART " : "C=WIT ");
  Serial.print(rightVal < threshold ? "R=ZWART" : "R=WIT");
  Serial.println();

  delay(300);
}

