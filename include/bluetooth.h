#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "BluetoothSerial.h"

void sendBT(BluetoothSerial& SerialBT, const void* data, size_t dataSize);
void receiveBT(BluetoothSerial& SerialBT, void* data, size_t dataSize);
void bluetoothTask(void* pvParameters);

#endif // BLUETOOTH_H