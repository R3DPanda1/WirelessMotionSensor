#ifndef BNO055_H
#define BNO055_H

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

struct BNO055Data {
    imu::Vector<3> accelerometer;  // x, y, z values in m/s^2
    imu::Vector<3> magnetometer;   // x, y, z values in microteslas
    imu::Vector<3> gyroscope;      // x, y, z values in rad/s
    imu::Vector<3> euler;          // orientation in degrees as yaw, roll, pitch
    imu::Quaternion quaternion;    // quaternion output for 3D orientation
    imu::Vector<3> linearAccel;    // linear acceleration (gravity subtracted)
    imu::Vector<3> gravity;        // gravity vector in m/s^2
    int8_t temperature;            // temperature in degrees Celsius
};

struct LinacQuatData {
  imu::Vector<3> linearAccel;
  imu::Quaternion orientation;
};

#endif // BNO055_H