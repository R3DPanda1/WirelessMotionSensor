#include "bluetooth.h"

BluetoothSerial SerialBT;
TaskHandle_t bluetoothRXTaskHandle = NULL;
TaskHandle_t bluetoothTXTaskHandle = NULL;

void sendStruct(BluetoothSerial &SerialBT, const char id, const void *data, size_t dataSize)
{
    uint8_t buffer[PACKET_SIZE] = {0}; // Initialize buffer with zeros
    buffer[0] = id;
    memcpy(buffer + 1, data, dataSize);
    SerialBT.write(buffer, sizeof(buffer));
    digitalWrite(BT_LED_PIN, TOGGLE_200MS_STATE);
}

void receiveStruct(BluetoothSerial &SerialBT)
{
    if (SerialBT.available() >= PACKET_SIZE)
    {
        uint8_t buffer[PACKET_SIZE];
        SerialBT.readBytes(buffer, PACKET_SIZE);

        char id = buffer[0];
        // Serial.write(id);
        // Serial.println();

        switch (id)
        {
        case FusionData_ID:
        {
            if (currentOperationMode != MODE_FUSION && btRole == SLAVE)
            {
                currentOperationMode = MODE_FUSION;
                displayNotification("Fusion");
            }
            FusionData receivedData;
            memcpy(&receivedData, buffer + 1, sizeof(receivedData));
            remoteImuData.timestamp = receivedData.timestamp;
            remoteImuData.linearAccel = receivedData.linearAccel;
            receivedData.rotation.normalize(); // make sure recieved quaternion is notmalized to prevent crashes
            remoteImuData.rotation = receivedData.rotation;
            break;
        }
        case TempData_ID:
        {
            if (currentOperationMode != MODE_TEMP && btRole == SLAVE)
            {
                currentOperationMode = MODE_TEMP;
                displayNotification("Temperature");
            }
            TempData receivedData;
            memcpy(&receivedData, buffer + 1, sizeof(receivedData));
            remoteImuData.timestamp = receivedData.timestamp;
            remoteImuData.temperature = receivedData.temperature;
            break;
        }
        case LevelData_ID:
        {
            if (currentOperationMode != MODE_LEVEL && btRole == SLAVE)
            {
                currentOperationMode = MODE_LEVEL;
                displayNotification("Spirit Level");
            }
            LevelData receivedData;
            memcpy(&receivedData, buffer + 1, sizeof(receivedData));
            remoteImuData.timestamp = receivedData.timestamp;
            receivedData.rotation.normalize(); // make sure recieved quaternion is notmalized to prevent crashes
            remoteImuData.rotation = receivedData.rotation;
            break;
        }
        case RawData_ID:
        {
            if (currentOperationMode != MODE_RAW && btRole == SLAVE)
            {
                currentOperationMode = MODE_RAW;
                displayNotification("Raw");
            }
            RawData receivedData;
            memcpy(&receivedData, buffer + 1, sizeof(receivedData));
            remoteImuData.timestamp = receivedData.timestamp;
            remoteImuData.accelerometer = receivedData.accelerometer;
            remoteImuData.magnetometer = receivedData.magnetometer;
            remoteImuData.gyroscope = receivedData.gyroscope;
            break;
        }
        case SyncStart_ID:
        {
            if (currentOperationMode != MODE_CLK_SYNC && btRole == SLAVE)
            {
                currentSyncMode = MODE_SYNC_START;
                currentOperationMode = MODE_CLK_SYNC;
            }
            unsigned long receivedData;
            memcpy(&receivedData, buffer + 1, sizeof(receivedData));
            remoteImuData.timestamp = receivedData;
            break;
        }
        default:
            displayNotification("Bluetooth Error!");
            unpairBT(SerialBT);
            break;
        }
        digitalWrite(BT_LED_PIN, TOGGLE_200MS_STATE);
    }
}

void sendBT(BluetoothSerial &SerialBT, const void *data, size_t dataSize)
{
    SerialBT.write((const uint8_t *)data, dataSize);
    digitalWrite(BT_LED_PIN, TOGGLE_200MS_STATE);
}

void receiveBT(BluetoothSerial &SerialBT, void *data, size_t dataSize)
{
    SerialBT.readBytes((char *)data, dataSize);
    // SerialBT.flush(); // flush any remaining or qued data
    digitalWrite(BT_LED_PIN, TOGGLE_200MS_STATE);
}

void unpairBT(BluetoothSerial &SerialBT)
{
    displayNotification("Disconnected!"); // show message early for quick response
    SerialBT.disconnect();
    SerialBT.flush();
    SerialBT.end();
    SerialBT.begin(bluetoothName, SLAVE); // Change bluetooth mode to slave
    btRole = UNPAIRED;
    currentBluetoothMode = MODE_DISCONNECTED;
    digitalWrite(BT_LED_PIN, LOW);
}

void bluetoothRXTask(void *pvParameters)
{
    BluetoothSerial SerialBT = (BluetoothSerial &)pvParameters;
    pinMode(BT_LED_PIN, OUTPUT);
    digitalWrite(BT_LED_PIN, LOW);
    for (;;)
    {
        if (SerialBT.connected())
        {
            if (SerialBT.available() >= PACKET_SIZE)
            {
                receiveStruct(SerialBT);
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS); // receiving should be very quick
    }
}

void bluetoothTXTask(void *pvParameters)
{
    BluetoothSerial SerialBT = (BluetoothSerial &)pvParameters;
    SerialBT.begin(bluetoothName, false);
    TickType_t xFrequency = pdMS_TO_TICKS(10); // Convert 10 ms to ticks (100 Hz)

    for (;;)
    {
        // Save wakeup time
        TickType_t xLastWakeTime = xTaskGetTickCount();
        xLastWakeTime = xTaskGetTickCount();

        if (currentBluetoothMode == MODE_DISCONNECT)
        {
            unpairBT(SerialBT);
        }
        else if (SerialBT.connected())
        {

            if (currentBluetoothMode != MODE_CONNECTED)
            {
                displayNotification("Connected!");
                currentBluetoothMode = MODE_CONNECTED;
            }
            switch (currentOperationMode)
            {
            case MODE_FUSION:
            {
                FusionData dataToSend = {localImuData.timestamp, localImuData.linearAccel, localImuData.rotation};
                sendStruct(SerialBT, FusionData_ID, &dataToSend, sizeof(dataToSend));
                break;
            }
            case MODE_TEMP:
            {
                TempData dataToSend = {localImuData.timestamp, localImuData.temperature};
                sendStruct(SerialBT, TempData_ID, &dataToSend, sizeof(dataToSend));
                vTaskDelay(990 / portTICK_PERIOD_MS);
                break;
            }
            case MODE_LEVEL:
            {
                LevelData dataToSend = {localImuData.timestamp, localImuData.rotation};
                sendStruct(SerialBT, LevelData_ID, &dataToSend, sizeof(dataToSend));
                break;
            }
            case MODE_RAW:
            {
                RawData dataToSend = {localImuData.timestamp, localImuData.accelerometer, localImuData.magnetometer, localImuData.gyroscope};
                sendStruct(SerialBT, RawData_ID, &dataToSend, sizeof(dataToSend));
                break;
            }
            case MODE_CLK_SYNC:
            {
                unsigned long dataToSend = syncedMillis();
                sendStruct(SerialBT, SyncStart_ID, &dataToSend, sizeof(dataToSend));
                break;
            }
            }
        }
        else
        {
            if (currentBluetoothMode == MODE_CONNECT)
            {
                SerialBT.end();
                SerialBT.begin(bluetoothName, MASTER); // Change bluetooth mode to master
                btRole = MASTER;
                // Try to connect to a device
                currentBluetoothMode = MODE_CONNECTING;
                if (SerialBT.connect(bluetoothName))
                {
                    displayNotification("Connected!");
                    currentBluetoothMode = MODE_CONNECTED;
                }
                else
                {
                    displayNotification("Couldn't connect!");
                    unpairBT(SerialBT);
                    currentBluetoothMode = MODE_DISCONNECT;
                }
            }
            else if (currentBluetoothMode != MODE_DISCONNECTED)
            {
                displayNotification("Connection lost!");
                unpairBT(SerialBT);
            }
        }
        // Delay until it is time to wake up again
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}