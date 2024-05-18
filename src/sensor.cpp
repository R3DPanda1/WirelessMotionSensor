#include "sensor.h"

TaskHandle_t sensorTaskHandle = NULL;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

void readSensor();

// Function to write an 8-bit value to a register
bool writeBNO(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(BNO055_ADDRESS);
    Wire.write(reg);                    // Register address
    Wire.write(value);                  // Value to write
    return Wire.endTransmission() == 0; // Return true if successful
}

// Function to read an 8-bit value from a register
uint8_t readBNO(uint8_t reg)
{
    uint8_t value = 0;
    Wire.beginTransmission(BNO055_ADDRESS);
    Wire.write(reg);                                       // Register address
    Wire.endTransmission(false);                           // Send restart condition
    Wire.requestFrom((uint8_t)BNO055_ADDRESS, (uint8_t)1); // Request 1 byte
    if (Wire.available())
    {
        value = Wire.read(); // Read the byte
    }
    return value;
}

void configureHighGInterrupt()
{
    bno.setMode(OPERATION_MODE_CONFIG);
    writeBNO(Adafruit_BNO055::BNO055_PAGE_ID_ADDR, 1);
    writeBNO(INT_MASK_ADDR, 0b00100000);
    writeBNO(INT_ADDR, 0b00100000);

    writeBNO(ACCEL_INTR_SETTINGS_ADDR, 0b01000000);

    writeBNO(ACCEL_HIGH_G_THRES_ADDR, 80);
    writeBNO(ACCEL_HIGH_G_DURN_ADDR, 0);

    writeBNO(Adafruit_BNO055::BNO055_PAGE_ID_ADDR, 0);

    // writeBNO(TEMP_SOURCE, 0b00000001); //choose the gyro temperature sensor
    // writeBNO(UNIT_SEL, 0b10010000); //get Fahrenheit

    bno.setMode(OPERATION_MODE_NDOF);
}

void sensorTask(void *pvParameters)
{
    if (!bno.begin()) // generates some errors: [Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
    {
        Serial.println(F("No BNO055 detected... Check wiring or I2C ADDR!"));
        vTaskDelete(NULL);
    }

    // int delayAmount = 10;
    TickType_t xFrequency = pdMS_TO_TICKS(10);      // Convert 10 ms to ticks (100 Hz)
    TickType_t xLastWakeTime = xTaskGetTickCount(); // Get the current tick

    for (;;)
    {
        if (currentOperationMode == MODE_TEMP)
        {
            xFrequency = pdMS_TO_TICKS(1000);
        }
        else
        {
            xFrequency = pdMS_TO_TICKS(10);
        }
        readSensor();

        if (currentBluetoothMode == MODE_CLK_SYNC)
        {
            configureHighGInterrupt();
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5000));
            currentBluetoothMode = MODE_CONNECTED;
        }

        // Delay until it is time to run again
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void readSensor()
{
    switch (currentOperationMode)
    {
    case MODE_LINACQUAD:
        localBnoData.timestamp = millis();
        localBnoData.orientation = bno.getQuat();
        localBnoData.linearAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        break;
    case MODE_LEVEL:
        localBnoData.timestamp = millis();
        localBnoData.orientation = bno.getQuat();
        //localBnoData.linearAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        break;
    case MODE_TEMP:
        localBnoData.timestamp = millis();
        localBnoData.temperature = bno.getTemp();
        break;
    default:

        break;
    }
}