#ifndef SD_CARD_H
#define SD_CARD_H

#include <SD.h>
#include <SPI.h>
#include <commons.h>

#define CS_PIN 5

#define CSV_BUFFER_SIZE 50 // at 100Hz = 2 writes per second to SD card

void sdCardTask(void *pvParameters);
void csvGenTask(void *pvParameters);
extern TaskHandle_t sdCardTaskHandle;
extern TaskHandle_t csvGenTaskHandle;
extern SemaphoreHandle_t csvSemaphore;
extern String currentFilePath;

#endif // SD_CARD_H