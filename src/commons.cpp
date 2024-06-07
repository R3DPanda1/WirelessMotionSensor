#include "commons.h"

volatile OperationMode currentOperationMode = MODE_FUSION;
volatile BluetoothMode currentBluetoothMode = MODE_DISCONNECTED;
volatile RecordingMode currentRecordingMode = IDLE;
volatile SyncMode currentSyncMode = MODE_IDLE;
volatile SD_State currentSdState = REMOVED;
volatile BluetoothConnectionState btRole = UNPAIRED;
SemaphoreHandle_t localImuSemaphore;
SemaphoreHandle_t remoteImuSemaphore;

IMU_Data localImuDataGL = {0};
IMU_Data remoteImuDataGL = {0};

unsigned long millis_sync_offset = 0;

//Testing
unsigned long test_timestamp = 0;
//unsigned long sentSyncTimestamp = 0;
//unsigned long reveivedSyncTimestampAt = 0;

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
  if (currentOperationMode == MODE_CLK_SYNC)
  {
    millis_sync_offset = millis();
    currentSyncMode = MODE_HIGH_G_DETECTED;
    detachInterrupt(HIGH_G_INT_PIN);
  }
}

void IRAM_ATTR test_isr()
{
  test_timestamp = syncedMillis();
  //millis_sync_offset = millis(); // perfect clock sync using common interrupt button
}