#ifndef COMMONS_H
#define COMMONS_H

/**
 * @file commons.h
 *
 * This file serves as a central place for common definitions and
 * declarations used throughout the project.
 */

#include "utility/imumaths.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <Arduino.h>

#define LIPO_MONITOR_PIN 33

#define TOGGLE_200MS_STATE ((syncedMillis() % 400 < 200) ? HIGH : LOW)

extern QueueHandle_t displayNotificationQueue;
void displayNotification(const char *message);

#define HIGH_G_INT_PIN 17
#define SYNC_BT_TOLERANCE 30 //maximum accepted time difference between devices
void IRAM_ATTR clk_sync_isr();
unsigned long syncedMillis();

enum SyncMode
{
    MODE_IDLE,
    MODE_SYNC_START,
    MODE_WAIT_HIGH_G,
    MODE_HIGH_G_DETECTED,
    MODE_RETRY,
    MODE_SYNC_SUCCESS
};
extern volatile SyncMode currentSyncMode;

const char FusionData_ID = 'F';
struct FusionData
{
    unsigned long timestamp;
    imu::Vector<3> linearAccel;
    imu::Quaternion orientation;
};

const char SyncStart_ID = 'S';

const char LevelData_ID = 'L';
struct LevelData
{
    unsigned long timestamp;
    imu::Quaternion orientation;
};

const char TempData_ID = 'T';
struct TempData
{
    unsigned long timestamp;
    int8_t temperature;
};

const char NoneData_ID = 'N';
struct NoneData
{
    int test;
};

struct BNO055Data
{
    unsigned long timestamp;
    imu::Vector<3> accelerometer;
    imu::Vector<3> magnetometer;
    imu::Vector<3> gyroscope;
    imu::Quaternion orientation;
    imu::Vector<3> linearAccel;
    int8_t temperature;
};

extern BNO055Data localBnoData;
extern BNO055Data remoteBnoData;

enum OperationMode
{
    MODE_FUSION,
    MODE_LEVEL,
    MODE_CLK_SYNC,
    MODE_TEMP
};
extern volatile OperationMode currentOperationMode;

enum BluetoothMode
{
    MODE_DISCONNECTED,
    MODE_DISCONNECT,
    MODE_CONNECT,
    MODE_CONNECTING,
    MODE_CONNECTED
};
extern volatile BluetoothMode currentBluetoothMode;
extern volatile uint8_t SD_inserted;

enum RecordingMode
{
    NONE,
    SD_CARD,
    UART
};
extern volatile RecordingMode currentRecordingMode;

// SemaphoreHandle_t sdSemaphore;

#endif // BLUETOOTH_H