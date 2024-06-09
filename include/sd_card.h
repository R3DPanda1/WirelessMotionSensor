#ifndef SD_CARD_H
#define SD_CARD_H

#include <SD.h>
#include <SPI.h>
#include <commons.h>

#define CS_PIN 5        // SPI chip select pin for the SD card
#define CD_PIN 14       // SD card detect pin

#define CSV_BUFFER_SIZE 50  // Buffer size for CSV data; at 100Hz = 2 writes per second to SD card

extern TaskHandle_t sdCardTaskHandle;  // Handle for the SD card task
extern TaskHandle_t csvGenTaskHandle;  // Handle for the CSV generation task
extern SemaphoreHandle_t csvSemaphore;  // Semaphore for synchronizing access to CSV buffer
extern String currentFilePath;  // Current file path for saving data

/**
 * @brief Task function for handling SD card operations
 * 
 * @param pvParameters Pointer to the parameters passed to the task
 */
void sdCardTask(void *pvParameters);

/**
 * @brief Task function for generating CSV data
 * 
 * @param pvParameters Pointer to the parameters passed to the task
 */
void csvGenTask(void *pvParameters);

#endif // SD_CARD_H
