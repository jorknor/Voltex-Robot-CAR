#include "BluetoothDriver.h"
char bluetoothInputString[BLUETOOTH_BUFFER_SIZE] = {0};
bool bluetoothStringComplete = false;
int bluetoothInputSize = 0;
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
      if (bluetoothInputSize < BLUETOOTH_BUFFER_SIZE){
    bluetoothInputString[bluetoothInputSize++] = inChar;
        }
    }
  }
}
