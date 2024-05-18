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

#define LIPO_MONITOR_PIN 33

#define TOGGLE_200MS_STATE ((millis() % 400 < 200) ? HIGH : LOW)

extern QueueHandle_t displayNotificationQueue;
void displayNotification(const char *message);

const char LinacQuatData_ID = 'M';
struct LinacQuatData
{
    unsigned long timestamp;
    imu::Vector<3> linearAccel;
    imu::Quaternion orientation;
};

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
    MODE_LINACQUAD,
    MODE_LEVEL,
    MODE_TEMP
};
extern volatile OperationMode currentOperationMode;

enum BluetoothMode
{
    MODE_DISCONNECTED,
    MODE_DISCONNECT,
    MODE_CONNECT,
    MODE_CONNECTED,
    MODE_CLK_SYNC
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