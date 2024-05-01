#include "bluetooth.h"

void sendBT(BluetoothSerial& SerialBT, const void *data, size_t dataSize)
{
  SerialBT.write((const uint8_t *)data, dataSize);
}

void receiveBT(BluetoothSerial& SerialBT, void *data, size_t dataSize)
{
  SerialBT.readBytes((char *)data, dataSize);
}

void bluetoothTask(void* pvParameters) {
    BluetoothSerial *SerialBT = (BluetoothSerial *)pvParameters;

  // Periodically check if the button is pressed
  bool connected;
  for (;;) {
    if (SerialBT->connected()) {

      // Evertlying in loop()

      //if (Serial.available()) {
      //  SerialBT.write(Serial.read());
      //}
      //if (SerialBT.available()) {
      //  Serial.write(SerialBT.read());dash
      //}
    } else {
      //isBTMaster = 0;
      /*
      if (digitalRead(BUTTON_PIN) == LOW) {
        SerialBT.end();
        SerialBT.begin(bluetoothName, true);
        Serial.printf("The device \"%s\" started in master mode, make sure slave BT device is on!\n", bluetoothName.c_str());
        isBTMaster = 1;
        // connect(address) is fast (up to 10 secs max), connect(bluetoothName) is slow (up to 30 secs max) as it needs
        // to resolve bluetoothName to address first, but it allows connecting to different devices with the same name.
        // Set CoreDebugLevel to Info to view devices Bluetooth address and device names
        connected = SerialBT.connect(bluetoothName);
        Serial.printf("Connecting to slave BT device named \"%s\"\n", bluetoothName.c_str());
        if (connected) {
          Serial.println("Connected Successfully!");
        } else {
          while (!SerialBT.connected(10000)) {
            Serial.println("Failed to connect. Make sure the remote device is available and in range, then restart the app.");
          }
        }
      }
      */
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Adjust delay as needed
  }
}