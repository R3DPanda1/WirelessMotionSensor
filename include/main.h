#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <rendering.h>
#include <bno055.h>
#include <bluetooth.h>
#include <modes.h>
#include <Wire.h>
//#include <utility/imumaths.h>
//#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define LIPO_MON_PIN 33
#define BT_LED_PIN 16
#define BNO_INT_PIN 17

BluetoothSerial SerialBT;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#endif // MAIN_H