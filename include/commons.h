#ifndef COMMONS_H
#define COMMONS_H

/**
 * @file commons.h
 *
 * This file serves as a central place for common definitions and 
 * declarations used throughout the project.
 */

#include "utility/imumaths.h"

const char LinacQuatData_ID = 'M';
struct LinacQuatData {
  imu::Vector<3> linearAccel;
  imu::Quaternion orientation;
};

struct BNO055Data {
    imu::Vector<3> accelerometer;  // x, y, z values in m/s^2
    imu::Vector<3> magnetometer;   // x, y, z values in microteslas
    imu::Vector<3> gyroscope;      // x, y, z values in rad/s
    imu::Vector<3> euler;          // orientation in degrees as yaw, roll, pitch
    imu::Quaternion orientation;    // quaternion output for 3D orientation
    imu::Vector<3> linearAccel;    // linear acceleration (gravity subtracted)
    imu::Vector<3> gravity;        // gravity vector in m/s^2
    int8_t temperature;            // temperature in degrees Celsius
};

extern BNO055Data localBnoData;
extern BNO055Data remoteBnoData;

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

#endif // BLUETOOTH_H