#include "sensor.h"

TaskHandle_t sensorTaskHandle = NULL;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

void readSensor();

void sensorTask(void *pvParameters)
{
    if (!bno.begin()) // generates some errors: [Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
    {
        Serial.println(F("No BNO055 detected... Check wiring or I2C ADDR!"));
        vTaskDelete(NULL);
    }

    int delayAmount = 10;
   // TickType_t xFrequency = pdMS_TO_TICKS(10);      // Convert 10 ms to ticks (100 Hz)
    //TickType_t xLastWakeTime = xTaskGetTickCount(); // Get the current tick

    for (;;)
    {
        if (currentOperationMode == MODE_LINACQUAD)
        {
            delayAmount = 10; // 100 Hz
        }
        else
        {
            delayAmount = 100; // 10 Hz
        }
        readSensor();
        // Delay until it is time to run again
        // vTaskDelayUntil(&xLastWakeTime, xFrequency);
        delay(delayAmount);
    }
}

void readSensor()
{
    localBnoData.orientation = bno.getQuat();
    localBnoData.linearAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
}