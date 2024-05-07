#include "bluetooth.h"

BluetoothSerial SerialBT;
TaskHandle_t bluetoothRXTaskHandle = NULL;
TaskHandle_t bluetoothTXTaskHandle = NULL;
BluetoothConnectionState btState = UNPAIRED;

void sendStruct(BluetoothSerial &SerialBT, const char id, const void *data, size_t dataSize)
{
    uint8_t buffer[PACKET_SIZE] = {0}; // Initialize buffer with zeros
    buffer[0] = id;
    memcpy(buffer + 1, data, dataSize);
    SerialBT.write(buffer, sizeof(buffer));
    digitalWrite(BT_LED_PIN, TOGGLE_500MS_STATE);
}

void receiveStruct(BluetoothSerial &SerialBT)
{
    if (SerialBT.available() >= PACKET_SIZE)
    {
        uint8_t buffer[PACKET_SIZE];
        SerialBT.readBytes(buffer, PACKET_SIZE);

        char id = buffer[0];
        //Serial.write(id);
        //Serial.println();

        switch (id)
        {
        case LinacQuatData_ID:
        {
            LinacQuatData receivedData;
            memcpy(&receivedData, buffer + 1, sizeof(LinacQuatData));
            remoteBnoData.linearAccel = receivedData.linearAccel;
            remoteBnoData.orientation = receivedData.orientation;
            break;
        }
        case STRUCT_B_ID:
        {
            StructB receivedDataB;
            memcpy(&receivedDataB, buffer + 1, sizeof(StructB));
            Serial.print("Received StructB: ");
            Serial.println(receivedDataB.str);
            break;
        }
        default:
            break;
        }
        digitalWrite(BT_LED_PIN, TOGGLE_500MS_STATE);
    }
}

void sendBT(BluetoothSerial &SerialBT, const void *data, size_t dataSize)
{
    SerialBT.write((const uint8_t *)data, dataSize);
    digitalWrite(BT_LED_PIN, TOGGLE_500MS_STATE);
}

void receiveBT(BluetoothSerial &SerialBT, void *data, size_t dataSize)
{
    SerialBT.readBytes((char *)data, dataSize);
    // SerialBT.flush(); // flush any remaining or qued data
    digitalWrite(BT_LED_PIN, TOGGLE_500MS_STATE);
}

void unpairBT(BluetoothSerial &SerialBT)
{
    SerialBT.disconnect();
    // SerialBT.flush();
    SerialBT.end();
    SerialBT.begin(bluetoothName, SLAVE); // Change bluetooth mode to slave
    btState = UNPAIRED;
    currentBluetoothMode = MODE_DISCONNECTED;
    digitalWrite(BT_LED_PIN, LOW);
    displayNotification("Disconnected!");
}

void bluetoothRXTask(void *pvParameters)
{
    BluetoothSerial SerialBT = (BluetoothSerial &)pvParameters;
    pinMode(BT_LED_PIN, OUTPUT);
    for (;;)
    {
        if (SerialBT.connected())
        {
            if (SerialBT.available() >= (sizeof(LinacQuatData) + 1))
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
    pinMode(BT_LED_PIN, OUTPUT);
    for (;;)
    {
        if (currentBluetoothMode == MODE_DISCONNECT)
        {
            Serial.println("Disconnecting!");
            unpairBT(SerialBT);
        }
        else if (SerialBT.connected())
        {

            if (currentBluetoothMode != MODE_CONNECTED)
            {
                displayNotification("Connected!");
                currentBluetoothMode = MODE_CONNECTED;
            }
            LinacQuatData dataToSend = {localBnoData.linearAccel, localBnoData.orientation};
            sendStruct(SerialBT, LinacQuatData_ID, &dataToSend, sizeof(LinacQuatData));
        }
        else
        {
            if (currentBluetoothMode == MODE_CONNECT)
            {
                SerialBT.end();
                SerialBT.begin(bluetoothName, MASTER); // Change bluetooth mode to master
                btState = MASTER;

                // Try to connect to a device
                if (SerialBT.connect(bluetoothName))
                {
                    Serial.println("Connected Successfully!");
                    displayNotification("Connected!");
                    currentBluetoothMode = MODE_CONNECTED;
                }
                else
                {
                    delay(1000);
                    if (SerialBT.connected())
                    {
                        Serial.println("Connected Successfully!");
                        displayNotification("Connected!");
                        currentBluetoothMode = MODE_CONNECTED;
                    }
                    else
                    {
                        Serial.println("Couldn't connect!");
                        displayNotification("Couldn't connect!");
                        unpairBT(SerialBT);
                    }
                }
            }
            else if (currentBluetoothMode != MODE_DISCONNECTED)
            {
                Serial.println("Connection lost!");
                displayNotification("Connection lost!");
                unpairBT(SerialBT);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}