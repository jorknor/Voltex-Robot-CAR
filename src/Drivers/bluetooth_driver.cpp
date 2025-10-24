String inputString = "";
bool stringComplete = false;
SoftwareSerial bluetooth(BRX, BTX);
void bluetoothInit() {
  pinMode(BRX, INPUT);
  pinMode(BTX, OUTPUT);
   // Set the baud rate for the SoftwareSerial object
    bluetooth.begin(9600);
}

void bluetoothUpdate() {
  while (bluetooth.available()) {
    // Get the new byte
    char inChar = (char)bluetooth.read();
  if (inChar == '\n') {
      // Set the flag
      stringComplete = true;
    }
    if (stringComplete == false){// Add it to the inputString
    inputString += inChar;
    }
  }
}
