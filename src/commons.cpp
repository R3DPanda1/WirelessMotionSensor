#include "commons.h"

volatile OperationMode currentOperationMode = MODE_LINACQUAD;
volatile BluetoothMode currentBluetoothMode = MODE_DISCONNECTED;
volatile RecordingMode currentRecordingMode = NONE;

BNO055Data localBnoData;
BNO055Data remoteBnoData;