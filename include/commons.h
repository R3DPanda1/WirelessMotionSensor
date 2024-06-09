#ifndef COMMONS_H
#define COMMONS_H

/**
 * @file commonsh
 *
 * This file serves as a central place for common definitions and
 * declarations used throughout the project
 */

#include "utility/imumaths.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <Arduino.h>

#define LIPO_MONITOR_PIN 33  // Pin for LiPo battery monitoring

#define TOGGLE_200MS_STATE ((syncedMillis() % 400 < 200) ? HIGH : LOW)  // Macro for toggling state every 200ms

extern QueueHandle_t displayNotificationQueue;

/**
 * @brief Displays a notification message on the OLED display
 * 
 * @param message The message to be displayed
 */
void displayNotification(const char *message);

// Testing only declarations
#define TEST_INT_PIN 15  // Pin for test interrupt
extern unsigned long test_timestamp;
void IRAM_ATTR test_isr();

extern unsigned long sentSyncTimestamp;
extern unsigned long reveivedSyncTimestampAt;

#define HIGH_G_INT_PIN 17  // Pin for high-g interrupt
#define SYNC_BT_TOLERANCE 10  // Maximum accepted time difference between devices in ms

/**
 * @brief Interrupt service routine for the clock synchronization process
 */
void IRAM_ATTR clk_sync_isr();

/**
 * @brief Gets the synchronized milliseconds
 * 
 * @return Synchronized milliseconds
 */
unsigned long syncedMillis();

#define SYNC_TIMEOUT 10000  // Amount of time to wait for time syncing after last detected hit (in ms)

/**
 * @brief Enum for synchronization modes
 */
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

/**
 * @brief Enum for SD card states
 */
enum SD_State
{
    REMOVED,
    INSERTED,
    FAILED,
    CONNECTED
};
extern volatile SD_State currentSdState;



/**
 * @brief Structure for Fusion Data
 */
const char FusionData_ID = 'F';
struct FusionData
{
    unsigned long timestamp;
    imu::Vector<3> linearAccel;
    imu::Quaternion orientation;
};


/**
 * @brief Structure for Raw Data
 */
const char RawData_ID = 'R';
struct RawData
{
    unsigned long timestamp;
    imu::Vector<3> accelerometer;
    imu::Vector<3> magnetometer;
    imu::Vector<3> gyroscope;
};

// Character ID for synchronization data
const char SyncStart_ID = 'S';

/**
 * @brief Structure for Spriit Level Data
 */
const char LevelData_ID = 'L';
struct LevelData
{
    unsigned long timestamp;
    imu::Quaternion orientation;
};

/**
 * @brief Structure for Temperature Data
 */
const char TempData_ID = 'T';
struct TempData
{
    unsigned long timestamp;
    int8_t temperature;
};

/**
 * @brief Structure for IMU Data
 */
struct IMU_Data
{
    unsigned long timestamp;
    imu::Vector<3> accelerometer;
    imu::Vector<3> magnetometer;
    imu::Vector<3> gyroscope;
    imu::Quaternion orientation;
    imu::Vector<3> linearAccel;
    int8_t temperature;
};

extern IMU_Data localImuDataGL;
extern IMU_Data remoteImuDataGL;

/**
 * @brief Enum for operation modes
 */
enum OperationMode
{
    MODE_FUSION,
    MODE_LEVEL,
    MODE_CLK_SYNC,
    MODE_TEMP,
    MODE_RAW
};
extern volatile OperationMode currentOperationMode;

/**
 * @brief Enum for Bluetooth modes
 */
enum BluetoothMode
{
    MODE_DISCONNECTED,
    MODE_DISCONNECT,
    MODE_CONNECT,
    MODE_CONNECTING,
    MODE_CONNECTED
};
extern volatile BluetoothMode currentBluetoothMode;

/**
 * @brief Enum for Bluetooth connection states
 */
enum BluetoothConnectionState
{
    UNPAIRED,
    MASTER = 1,
    SLAVE = 0
};
extern volatile BluetoothConnectionState btRole;

/**
 * @brief Enum for recording modes
 */
enum RecordingMode
{
    IDLE,
    CREATE_FILE,
    RECORDING
};
extern volatile RecordingMode currentRecordingMode;

extern SemaphoreHandle_t localImuSemaphore;
extern SemaphoreHandle_t remoteImuSemaphore;

#endif // COMMONS_H