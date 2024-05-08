#include <sd_card.h>

String currentFilePath;
TaskHandle_t sdCardTaskHandle;
TaskHandle_t csvGenTaskHandle;
SemaphoreHandle_t csvSemaphore = NULL;
String csvDataToWrite;
uint8_t dataReady = 1;
uint8_t noSD = 0;

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

String vectorToCsvString(imu::Vector<3> vector)
{
    return (String(vector.x()) + ";" + String(vector.y()) + ";" + String(vector.z()) + ";");
}

// Define a function that formats sensor data into a CSV string.
String formatSensorData(BNO055Data &data)
{
    String result = String(data.timestamp) + ";";
    result += vectorToCsvString(data.accelerometer);
    result += vectorToCsvString(data.magnetometer);
    result += vectorToCsvString(data.gyroscope);
    result += String(data.orientation.w()) + ";" + String(data.orientation.x()) + ";" + String(data.orientation.y()) + ";" + String(data.orientation.z()) + ";";
    result += vectorToCsvString(data.linearAccel);
    result += String(data.temperature) + ";";
    return result;
}

void csvGenTask(void *pvParameters)
{
    for (;;)
    {
        while (currentRecordingMode == SD_CARD)
        {
            String dataBuffer; // String to hold CSV formatted data

            for (int i = 0; i < CSV_BUFFER_SIZE; i++)
            {
                dataBuffer += formatSensorData(localBnoData);

                // Handle Bluetooth mode-specific operations
                if (currentBluetoothMode == MODE_CONNECTED)
                {
                    dataBuffer += formatSensorData(remoteBnoData);
                }

                dataBuffer += "\n";
                vTaskDelay(pdMS_TO_TICKS(10));
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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void sdCardTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(1000)); // if SD card is initialized too early, esp32 reboots

    while (!SD.begin(CS_PIN)) // check if SD card is present
    {
        if (noSD == 0)
        {
            displayNotification("No SD card detected");
            noSD = 1;
            vTaskDelete(NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    displayNotification("SD card inserted");
    SD_inserted = 1;
    while (currentRecordingMode != SD_CARD)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Generate new filename
    currentFilePath = generateNewFilename();

    // Open new file
    File myFile = SD.open(currentFilePath.c_str(), FILE_WRITE);
    if (myFile)
    {
        // Timestamp, Acc, Mag, Gyro, Quat, linAcc, Temp
        myFile.print("Timestamp;AccX;AccY;AccZ;MagX;MagY;MagZ;GyroX;GyroY;GyroZ;QuatW;QuatX;QuatY;QuatZ;linAccX;linAccY;linAccZ;Temp;");  // Write header
        myFile.println("Timestamp;AccX;AccY;AccZ;MagX;MagY;MagZ;GyroX;GyroY;GyroZ;QuatW;QuatX;QuatY;QuatZ;linAccX;linAccY;linAccZ;Temp"); // Write header
        myFile.close();
        vTaskDelay(pdMS_TO_TICKS(1000));
        displayNotification(currentFilePath.c_str());
    }
    else
    {
        displayNotification("Error creating file");
        while (true)
        {
        };
    }
    csvSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(csvGenTask, "csvGeneratorTask", 2048, NULL, 2, &csvGenTaskHandle);
    xSemaphoreGive(csvSemaphore);

    for (;;)
    {
        // TickType_t xFrequency = pdMS_TO_TICKS(10);      // Convert 10 ms to ticks (100 Hz)
        // TickType_t xLastWakeTime = xTaskGetTickCount(); // Get the current tick
        if (dataReady == 1)
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
        vTaskDelay(pdMS_TO_TICKS(5));
        // vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}