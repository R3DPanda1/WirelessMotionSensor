#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "commons.h"

#define BT_SW_PIN 26       // Pin number for Bluetooth switch
#define REC_SW_PIN 25      // Pin number for Record switch
#define MODE_SW_PIN 27     // Pin number for Mode switch
#define CLK_SYNC_SW_PIN 32 // Pin number for Clock Sync switch

/**
 * @brief Task function for handling button inputs
 * 
 * @param pvParameters Pointer to the parameters passed to the task
 */
void buttonTask(void* pvParameters);

extern TaskHandle_t buttonTaskHandle;

#endif // BUTTON_H