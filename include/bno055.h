#ifndef BNO055_H
#define BNO055_H

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

struct LinacQuatData {
  float x;
  float y;
  float z;
  double qW;
  double qX;
  double qY;
  double qZ;
};

struct Quat {
  double qW;
  double qX;
  double qY;
  double qZ;
};

Quat multiplyQuaternions(Quat quat1, Quat quat2);

#endif // BNO055_H