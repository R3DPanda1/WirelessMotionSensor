#ifndef BLUETOOTH_H
#define BLUETOOTH_H

//lower connection timeout of the library
#include "BluetoothSerial.h"

#include "commons.h"
#define PACKET_SIZE 68  // Bluetooth packet size (must be higher than the biggest struct that can be sent and <=512)
#define BT_LED_PIN 16
// used to indicate BT communication without toggling too fast

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

const String bluetoothName = "WirelessMotionSensor";

enum BluetoothConnectionState {
    UNPAIRED,
    MASTER = 1,
    SLAVE = 0
};

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