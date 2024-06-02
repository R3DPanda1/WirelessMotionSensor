#include "sensor.h"

TaskHandle_t sensorTaskHandle = NULL;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
bool calibrationSaved = false;

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

void saveCalibrationData()
{
    uint8_t calData[22];
    // Read calibration data
    bno.getSensorOffsets(calData);

    // Save calibration data
    for (int i = 0; i < 22; i++)
    {
        EEPROM.write(i, calData[i]);
    }

    // Commit changes to EEPROM
    EEPROM.commit();
    // displayNotification("Calibration saved");
}

void loadCalibrationData()
{
    uint8_t calData[22];

    // Read calibration data from EEPROM
    for (int i = 0; i < 22; i++)
    {
        calData[i] = EEPROM.read(i);
    }

    // Load calibration data
    bno.setSensorOffsets(calData);
    // displayNotification("Calibration load");
}

void configureHighGInterrupt()
{
    bno.setMode(OPERATION_MODE_CONFIG);

    // clear previous interrupt
    (void)readBNO(INT_STA);
    writeBNO(Adafruit_BNO055::BNO055_SYS_TRIGGER_ADDR, 0b01000000);

    writeBNO(Adafruit_BNO055::BNO055_PAGE_ID_ADDR, 1);
    writeBNO(INT_MASK_ADDR, 0b00100000);
    writeBNO(INT_ADDR, 0b00100000);

    writeBNO(ACCEL_INT_SETTINGS_ADDR, 0b11100000);

    writeBNO(ACCEL_HIGH_G_THRES_ADDR, 80);
    writeBNO(ACCEL_HIGH_G_DURN_ADDR, 0);

    writeBNO(Adafruit_BNO055::BNO055_PAGE_ID_ADDR, 0);

    // writeBNO(TEMP_SOURCE, 0b00000001); //choose the gyro temperature sensor
    // writeBNO(UNIT_SEL, 0b10010000); //get Fahrenheit

    bno.setMode(OPERATION_MODE_NDOF);
}

void clearInterrupt()
{
    bno.setMode(OPERATION_MODE_CONFIG);

    // clear previous interrupt
    (void)readBNO(INT_STA);
    writeBNO(Adafruit_BNO055::BNO055_SYS_TRIGGER_ADDR, 0b01000000);

    bno.setMode(OPERATION_MODE_NDOF);
}

void sensorTask(void *pvParameters)
{
    if (!bno.begin()) // generates some errors: [Wire.cpp:499] requestFrom(): i2cWriteReadNonStop returned Error -1
    {
        Serial.println(F("No BNO055 detected... Check wiring or I2C ADDR!"));
        vTaskDelete(NULL);
    }

    /* Use external crystal for better accuracy */
    // bno.setExtCrystalUse(true);

    // int delayAmount = 10;
    TickType_t xFrequency = pdMS_TO_TICKS(10); // Convert 10 ms to ticks (100 Hz)
    unsigned long sync_timeout_time = 0;

    loadCalibrationData();

    for (;;)
    {
        //Save wakeup time
        TickType_t xLastWakeTime = xTaskGetTickCount();
        xLastWakeTime = xTaskGetTickCount();

        if (calibrationSaved == false && millis() >= CALIBRATION_SAVE_TIME)
        {
            saveCalibrationData();
            calibrationSaved = true;
        }

        if (currentOperationMode == MODE_CLK_SYNC)
        {
            if (currentBluetoothMode == MODE_CONNECTED)
            {
                if (currentSyncMode == MODE_SYNC_START)
                {
                    displayNotification("Sync started");
                    configureHighGInterrupt();
                    while (digitalRead(HIGH_G_INT_PIN) == HIGH)
                    {
                        clearInterrupt();
                        delay(100);
                    }
                    attachInterrupt(HIGH_G_INT_PIN, clk_sync_isr, RISING);
                    sync_timeout_time = millis() + SYNC_TIMEOUT;
                    currentSyncMode = MODE_WAIT_HIGH_G;
                }
                if (currentSyncMode == MODE_RETRY)
                {
                    clearInterrupt();
                    while (digitalRead(HIGH_G_INT_PIN) == HIGH)
                    {
                        clearInterrupt();
                        delay(100);
                    }
                    attachInterrupt(HIGH_G_INT_PIN, clk_sync_isr, RISING);
                    sync_timeout_time = millis() + SYNC_TIMEOUT;
                    currentSyncMode = MODE_WAIT_HIGH_G;
                }
                // Wait for interrupt with a timeout
                while (currentSyncMode == MODE_WAIT_HIGH_G && millis() < sync_timeout_time)
                {
                    delay(100);
                }
                switch (currentSyncMode)
                {
                case MODE_WAIT_HIGH_G: // Still waiting after timeout
                    displayNotification("Sync failed");
                    currentSyncMode = MODE_IDLE;
                    detachInterrupt(HIGH_G_INT_PIN);
                    currentOperationMode = MODE_FUSION;
                    break;
                case MODE_HIGH_G_DETECTED:
                {
                    if (btRole == MASTER)
                    {
                        // check if the remote device was synced roughly at the same time
                        unsigned long remoteTime = remoteImuData.timestamp;
                        unsigned long localTime = syncedMillis();
                        unsigned long timeDiff;
                        if (localTime > remoteTime)
                        {
                            timeDiff = localTime - remoteTime;
                        }
                        else
                        {
                            timeDiff = remoteTime - localTime;
                        }
                        if (timeDiff < SYNC_BT_TOLERANCE)
                        {
                            currentSyncMode = MODE_SYNC_SUCCESS;
                        }
                        else
                        {
                            displayNotification("Try again!");
                            currentSyncMode = MODE_RETRY;
                        }
                    }
                    else
                    {
                        displayNotification("Hit!");
                        currentSyncMode = MODE_RETRY;
                    }
                    break;
                }
                case MODE_SYNC_SUCCESS:
                    displayNotification("Synced!");
                    currentSyncMode = MODE_IDLE;
                    detachInterrupt(HIGH_G_INT_PIN);
                    currentOperationMode = MODE_FUSION;
                    break;
                }
            }
            else
            {
                currentOperationMode == MODE_FUSION;
                detachInterrupt(HIGH_G_INT_PIN);
            }
        }
        else
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
        }
        // Delay until it is time to wake up again
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void readSensor()
{
    switch (currentOperationMode)
    {
    case MODE_FUSION:
        localImuData.timestamp = syncedMillis();
        localImuData.rotation = bno.getQuat();
        localImuData.rotation.normalize(); // make sure recieved quaternion is notmalized to prevent crashes
        localImuData.linearAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        break;
    case MODE_LEVEL:
        localImuData.timestamp = syncedMillis();
        localImuData.rotation = bno.getQuat();
        localImuData.rotation.normalize(); // make sure recieved quaternion is notmalized to prevent crashes
        // localBnoData.linearAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        break;
    case MODE_TEMP:
        localImuData.timestamp = syncedMillis();
        localImuData.temperature = bno.getTemp();
        break;
    case MODE_RAW:
        localImuData.timestamp = syncedMillis();
        localImuData.accelerometer = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        localImuData.gyroscope = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        localImuData.magnetometer = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
        break;
    default:

        break;
    }
}