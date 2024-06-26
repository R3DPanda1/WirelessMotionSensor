#ifndef SENSOR_H
#define SENSOR_H

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "button.h"
#include <EEPROM.h>
//#include "commons.h"

// Register addresses not included in the adafruit library
/* Interrupt registers*/
#define INT_MASK_ADDR 0x0F
#define INT_ADDR 0x10
#define INT_STA 0x37
#define ACCEL_ANY_MOTION_THRES_ADDR 0x11
#define ACCEL_INT_SETTINGS_ADDR 0x12
#define ACCEL_HIGH_G_DURN_ADDR 0x13
#define ACCEL_HIGH_G_THRES_ADDR 0x14
//masks to set specific axis to the interrupt functions
#define ACCEL_HIGH_G_X_AXIS_MSK (0x20)
#define ACCEL_HIGH_G_Y_AXIS_MSK (0x40)
#define ACCEL_HIGH_G_Z_AXIS_MSK (0x80)
#define BNO055_ACC_CONFIG_ADDR (0x08)  //address of register to change operatingmode/bandwidth and G-range for acceleration
#define UNIT_SEL 0x3B
#define TEMP_SOURCE 0x40

#define CALIBRATION_SAVE_TIME 60000 //time in ms when calibration data is saved to flash memory

#define BNO055_ADDRESS 0x28 // BNO055 I2C address

extern TaskHandle_t sensorTaskHandle; // Handle for the sensor task
extern Adafruit_BNO055 bno; // BNO055 object

/**
 * @brief Task function for interfacing with the IMU
 * 
 * @param pvParameters Pointer to the parameters passed to the task
 */
void sensorTask(void* pvParameters);

#endif // SENSOR_H