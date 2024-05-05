#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "commons.h"

#define BT_SW_PIN 26
#define REC_SW_PIN 25
#define MODE_SW_PIN 27
#define CLK_SYNC_SW_PIN 32

void buttonTask(void* pvParameters);
extern TaskHandle_t buttonTaskHandle;

#endif // BUTTON_H