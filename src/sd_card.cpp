#include <sd_card.h>

String currentFilePath;
TaskHandle_t sdCardTaskHandle;
TaskHandle_t csvGenTaskHandle;
SemaphoreHandle_t csvSemaphore = NULL;
String csvDataToWrite;
uint8_t dataReady = 1;
uint8_t csvGenTaskCreated = 0;

void WriteFile(const char *path, const char *data)
{
    File myFile = SD.open(path, FILE_APPEND); // Open file in append mode
    if (myFile)
    {
        myFile.print(data); // Write data to file
        myFile.close();
    }
    else
    {
        displayNotification("Error opening file");
    }
}

int countFiles()
{
    int count = 0;
    File root = SD.open("/");
    File file = root.openNextFile();
    while (file)
    {
        if (!file.isDirectory())
        {
            count++;
        }
        file = root.openNextFile();
    }
    return count;
}

String generateNewFilename()
{
    int fileCount = countFiles();
    return String("/data") + String(fileCount) + String(".csv");
}

char *vectorToCsvString(imu::Vector<3> vector, char *buffer)
{
    snprintf(buffer, 50, "%.6f,%.6f,%.6f,", vector.x(), vector.y(), vector.z());
    return buffer;
}

String formatSensorData(IMU_Data &data)
{
    char buffer[500]; // Buffer to hold the entire CSV line.

    // Buffers for individual vector conversions.
    char accBuffer[35];
    char magBuffer[35];
    char gyroBuffer[35];
    char laccBuffer[35];
    snprintf(buffer, sizeof(buffer), "%lu,%s%s%s%.6f,%.6f,%.6f,%.6f,%s%d,",
             data.timestamp,
             vectorToCsvString(data.accelerometer, accBuffer),
             vectorToCsvString(data.magnetometer, magBuffer),
             vectorToCsvString(data.gyroscope, gyroBuffer),
             data.rotation.w(), data.rotation.x(), data.rotation.y(), data.rotation.z(),
             vectorToCsvString(data.linearAccel, laccBuffer),
             data.temperature);

    return String(buffer); // Convert the entire buffer to a String object just once before returning.
}

void csvGenTask(void *pvParameters)
{
    for (;;)
    {
        TickType_t xFrequency = pdMS_TO_TICKS(10); // Convert 10 ms to ticks (100 Hz)
        while (currentRecordingMode == RECORDING)
        {
            String dataBuffer; // String to hold CSV formatted data
            TickType_t xLastWakeTime = xTaskGetTickCount(); // Get the current tick

            for (int i = 0; i < CSV_BUFFER_SIZE; i++)
            {
                dataBuffer += formatSensorData(localImuData);

                // Handle Bluetooth mode-specific operations
                if (currentBluetoothMode == MODE_CONNECTED)
                {
                    dataBuffer += formatSensorData(remoteImuData);
                }

                dataBuffer += "\n";
                vTaskDelayUntil(&xLastWakeTime, xFrequency);
            }

            if (xSemaphoreTake(csvSemaphore, portMAX_DELAY) == pdTRUE)
            {
                // send String to sdCardTask
                csvDataToWrite = dataBuffer;
                dataBuffer.clear();
                dataReady = 1;
                xSemaphoreGive(csvSemaphore);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // delay when not recording
    }
}

void sdCardTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(1000)); // if SD card is initialized too early, ESP32 crashes
    pinMode(CD_PIN, INPUT_PULLUP);
    TickType_t xFrequency = pdMS_TO_TICKS(10);      // Convert 10 ms to ticks (100 Hz)
    for (;;)
    {
        TickType_t xLastWakeTime = xTaskGetTickCount(); // Get the current tick
        switch (currentSdState)
        {
        case REMOVED:
            if (digitalRead(CD_PIN) == LOW) // check if inserted
            {
                displayNotification("SD inserted");
                currentSdState = INSERTED;
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;
        case INSERTED:
            if (SD.begin(CS_PIN)) // check if SD card is working
            {
                currentSdState = CONNECTED;
            }
            else
            {
                displayNotification("Can't connect SD");
                currentSdState = FAILED;
            }
            break;
        case CONNECTED:
            switch (currentRecordingMode)
            {
            case IDLE:
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            case CREATE_FILE:
            {
                // Generate new filename
                currentFilePath = generateNewFilename();

                // Open new file
                File myFile = SD.open(currentFilePath.c_str(), FILE_WRITE);
                if (myFile)
                {
                    myFile.print("Timestamp;AccX;AccY;AccZ;MagX;MagY;MagZ;GyroX;GyroY;GyroZ;QuatW;QuatX;QuatY;QuatZ;linAccX;linAccY;linAccZ;Temp;");  // Write header
                    myFile.println("Timestamp;AccX;AccY;AccZ;MagX;MagY;MagZ;GyroX;GyroY;GyroZ;QuatW;QuatX;QuatY;QuatZ;linAccX;linAccY;linAccZ;Temp"); // Write header
                    myFile.close();
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    displayNotification(currentFilePath.c_str());
                    if (csvGenTaskCreated == 0) // avoid recreation
                    {
                        csvSemaphore = xSemaphoreCreateBinary();
                        xTaskCreate(csvGenTask, "csvGeneratorTask", 4096, NULL, 4, &csvGenTaskHandle);
                        xSemaphoreGive(csvSemaphore);
                        csvGenTaskCreated = 1;
                    }
                }
                else
                {
                    displayNotification("Error creating file");
                    currentSdState = FAILED;
                }
                vTaskDelay(pdMS_TO_TICKS(100));
                currentRecordingMode = RECORDING;
                break;
            }
            case RECORDING:
                if (dataReady == 1)
                {
                    if (digitalRead(CD_PIN) == LOW)
                    {
                        if (xSemaphoreTake(csvSemaphore, portMAX_DELAY) == pdTRUE)
                        {
                            // write to SD card
                            WriteFile(currentFilePath.c_str(), csvDataToWrite.c_str());
                            csvDataToWrite.clear();
                            dataReady = 0;
                            xSemaphoreGive(csvSemaphore);
                        }
                    }
                    else
                    {
                        displayNotification("SD card removed!");
                        currentSdState = REMOVED;
                        currentRecordingMode = IDLE;
                        SD.end();
                    }
                }
                break;
            }
            if (digitalRead(CD_PIN) == HIGH)
            {
                currentSdState = REMOVED;
                currentRecordingMode = IDLE;
                SD.end();
                displayNotification("SD card removed!");
            }
            break;
        case FAILED:
            if (digitalRead(CD_PIN) == HIGH)
            {
                currentSdState = REMOVED;
                currentRecordingMode = IDLE;
                SD.end();
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            break;
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}