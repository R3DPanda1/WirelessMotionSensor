#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "BluetoothSerial.h"

const String bluetoothName = "WirelessMotionSensor";

enum BluetoothConnectionState {
    UNPAIRED,
    MASTER = 1,
    SLAVE = 0
};
extern BluetoothConnectionState btState;

void sendBT(BluetoothSerial& SerialBT, const void* data, size_t dataSize);
void receiveBT(BluetoothSerial& SerialBT, void* data, size_t dataSize);
void bluetoothTask(void* pvParameters);
extern TaskHandle_t bluetoothTaskHandle;

#endif // BLUETOOTH_H