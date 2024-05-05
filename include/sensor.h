#ifndef SENSOR_H
#define SENSOR_H

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "button.h"

void sensorTask(void* pvParameters);
extern TaskHandle_t sensorTaskHandle;
extern Adafruit_BNO055 bno;

#endif // SENSOR_H