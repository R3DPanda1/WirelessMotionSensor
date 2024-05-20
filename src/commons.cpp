#include "commons.h"

volatile OperationMode currentOperationMode = MODE_FUSION;
volatile BluetoothMode currentBluetoothMode = MODE_DISCONNECTED;
volatile RecordingMode currentRecordingMode = IDLE;
volatile SyncMode currentSyncMode = MODE_IDLE;
volatile SD_State currentSdState = REMOVED;
volatile BluetoothConnectionState btRole = UNPAIRED;

BNO055Data localBnoData = {0};
BNO055Data remoteBnoData = {0};

unsigned long millis_sync_offset = 0;


unsigned long syncedMillis()
{
  return millis() - millis_sync_offset;
}

void displayNotification(const char *message)
{
  if (uxQueueSpacesAvailable(displayNotificationQueue) > 0)
  {                              // Check if there's space in the queue
    char *msg = strdup(message); // Duplicate the message to send
    if (xQueueSend(displayNotificationQueue, &msg, 0) != pdPASS)
    {
      free(msg); // Prevent memory leak if queue send fails
    }
  }
}

void IRAM_ATTR clk_sync_isr()
{
  millis_sync_offset = millis();
  currentSyncMode = MODE_HIGH_G_DETECTED;
  detachInterrupt(HIGH_G_INT_PIN);
}