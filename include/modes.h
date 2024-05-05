#ifndef MODES_H
#define MODES_H

#include <Arduino.h>

#define BT_SW_PIN 26
#define REC_SW_PIN 25
#define MODE_SW_PIN 27
#define CLK_SYNC_SW_PIN 32

void buttonTask(void* pvParameters);
extern TaskHandle_t buttonTaskHandle;

enum OperationMode {
    MODE_LINACQUAD,
    MODE_NONE
};
extern volatile OperationMode currentOperationMode;

enum BluetoothMode {
    MODE_DISCONNECTED,
    MODE_DISCONNECT,
    MODE_CONNECT,
    MODE_SENDER,
    MODE_RECEIVER,
    MODE_CLK_SYNC
};
extern volatile BluetoothMode currentBluetoothMode;

enum RecordingMode {
    NONE,
    SD_CARD,
    UART
};
extern volatile RecordingMode currentRecordingMode;

#endif // MODES_H