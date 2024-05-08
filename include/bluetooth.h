#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "BluetoothSerial.h"
#include "commons.h"
#define PACKET_SIZE 96  // Bluetooth packet size (must be higher than the biggest struct that can be sent and <=512)
#define BT_LED_PIN 16
// used to indicate BT communication without toggling too fast

const String bluetoothName = "WirelessMotionSensor";

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
void bluetoothRXTask(void* pvParameters);
void bluetoothTXTask(void* pvParameters);
void unpairBT(BluetoothSerial &SerialBT);
extern TaskHandle_t bluetoothRXTaskHandle;
extern TaskHandle_t bluetoothTXTaskHandle;
extern BluetoothSerial SerialBT;

#endif // BLUETOOTH_H