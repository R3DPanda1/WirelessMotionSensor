#include "bluetooth.h"

BluetoothSerial SerialBT;
TaskHandle_t bluetoothRXTaskHandle = NULL;
TaskHandle_t bluetoothTXTaskHandle = NULL;
BluetoothConnectionState btState = UNPAIRED;

// Send data with identifier
void sendStruct(BluetoothSerial &SerialBT, const char id, const void *data, size_t dataSize)
{
    uint8_t buffer[dataSize + 1];
    buffer[0] = id;
    memcpy(buffer + 1, data, dataSize);
    SerialBT.write(buffer, sizeof(buffer));
    // Serial.print("Sent:");
    // Serial.write(buffer, sizeof(buffer));
    // Serial.println();
    digitalWrite(BT_LED_PIN, TOGGLE_500MS_STATE);
}

// Receive data and parse based on identifier
void receiveStruct(BluetoothSerial &SerialBT)
{

    // Read the identifier character
    char id = SerialBT.read();
    // Serial.print("Received:");
    // Serial.write(id);
    // Serial.println();

    // Process data based on the identifier
    switch (id)
    {
    case LinacQuatData_ID:
    {
        LinacQuatData receivedData;
        SerialBT.readBytes(reinterpret_cast<char *>(&receivedData), sizeof(LinacQuatData));
        remoteBnoData.linearAccel = receivedData.linearAccel;
        remoteBnoData.orientation = receivedData.orientation;
        break;
    }
    case STRUCT_B_ID:
    {
        StructB receivedData;
        SerialBT.readBytes(reinterpret_cast<char *>(&receivedData), sizeof(StructB));

        Serial.print("Received StructB: ");
        Serial.println(receivedData.str);
        break;
    }
    default:
        // Unknown identifier, discard the message
        break;
    }
    // SerialBT.flush(); // flush any remaining or qued data
    digitalWrite(BT_LED_PIN, TOGGLE_500MS_STATE);
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
            if (SerialBT.available())
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
                    currentBluetoothMode = MODE_CONNECTED;
                }
                else
                {
                    delay(1000);
                    if (SerialBT.connected())
                    {
                        Serial.println("Connected Successfully!");
                        currentBluetoothMode = MODE_CONNECTED;
                    }
                    else
                    {
                        Serial.println("Couldn't connect!");
                        unpairBT(SerialBT);
                    }
                }
                Serial.printf("Connecting as the master, make sure a \"%s\" device is on!\n", bluetoothName.c_str());
                Serial.println("Connected Successfully!");
            }
            else if (currentBluetoothMode != MODE_DISCONNECTED)
            {
                Serial.println("Connection lost!");
                unpairBT(SerialBT);
            }
        }
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}