#include "commons.h"

volatile OperationMode currentOperationMode = MODE_LINACQUAD;
volatile BluetoothMode currentBluetoothMode = MODE_DISCONNECTED;
volatile RecordingMode currentRecordingMode = NONE;
volatile uint8_t SD_inserted = 0;

BNO055Data localBnoData;
BNO055Data remoteBnoData;

void displayNotification(const char* message) {
  if (uxQueueSpacesAvailable(displayNotificationQueue) > 0) {  // Check if there's space in the queue
    char* msg = strdup(message);  // Duplicate the message to send
    if (xQueueSend(displayNotificationQueue, &msg, 0) != pdPASS) {
      free(msg);  // Prevent memory leak if queue send fails
    }
  }
}