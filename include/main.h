#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <rendering.h>
#include <sensor.h>
#include <bluetooth.h>
#include <button.h>
#include <Wire.h>
//#include <utility/imumaths.h>
//#include <math.h>
#include "commons.h"

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define LIPO_MON_PIN 33
#define BNO_INT_PIN 17

#endif // MAIN_H