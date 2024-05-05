#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "BluetoothSerial.h"
#include "commons.h"

#define BT_LED_PIN 16

const String bluetoothName = "WirelessMotionSensor";

// Define identifier characters for different types of messages
const char STRUCT_B_ID = 'B';

// Define the structures you want to send
struct StructA {
  int value1;
  float value2;
};

struct StructB {
  char str[20];
};

enum BluetoothConnectionState {
    UNPAIRED,
    MASTER = 1,
    SLAVE = 0
};
extern BluetoothConnectionState btState;

void sendBT(BluetoothSerial& SerialBT, const void* data, size_t dataSize);
void receiveBT(BluetoothSerial& SerialBT, void* data, size_t dataSize);
void sendStruct(BluetoothSerial &SerialBT, const char id, const void *data, size_t dataSize);
void receiveStruct(BluetoothSerial &SerialBT);
void bluetoothTask(void* pvParameters);
void unpairBT(BluetoothSerial &SerialBT);
extern TaskHandle_t bluetoothTaskHandle;
extern BluetoothSerial SerialBT;

#endif // BLUETOOTH_H