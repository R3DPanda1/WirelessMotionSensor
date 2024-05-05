#include "bluetooth.h"

BluetoothSerial SerialBT;
TaskHandle_t bluetoothTaskHandle = NULL;
BluetoothConnectionState btState = UNPAIRED;

// Send data with identifier
void sendStruct(BluetoothSerial &SerialBT, const char id, const void *data, size_t dataSize)
{
    uint8_t buffer[dataSize + 1];
    buffer[0] = id;
    memcpy(buffer + 1, data, dataSize);
    SerialBT.write(buffer, sizeof(buffer));
    digitalWrite(BT_LED_PIN, !digitalRead(BT_LED_PIN));
}

// Receive data and parse based on identifier
void receiveStruct(BluetoothSerial &SerialBT)
{
    // Read the identifier character
    char id = SerialBT.read();
    Serial.write(id);
    Serial.println();

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
    digitalWrite(BT_LED_PIN, !digitalRead(BT_LED_PIN));
}

void sendBT(BluetoothSerial &SerialBT, const void *data, size_t dataSize)
{
    SerialBT.write((const uint8_t *)data, dataSize);
    digitalWrite(BT_LED_PIN, !digitalRead(BT_LED_PIN));
}

void receiveBT(BluetoothSerial &SerialBT, void *data, size_t dataSize)
{
    SerialBT.readBytes((char *)data, dataSize);
    SerialBT.flush(); // flush any remaining or qued data
    digitalWrite(BT_LED_PIN, !digitalRead(BT_LED_PIN));
}

void unpairBT(BluetoothSerial &SerialBT)
{
    SerialBT.disconnect();
    SerialBT.flush();
    SerialBT.end();
    SerialBT.begin(bluetoothName, SLAVE); // Change bluetooth mode to slave
    btState = UNPAIRED;
    currentBluetoothMode = MODE_DISCONNECTED;
    digitalWrite(BT_LED_PIN, LOW);
}

void bluetoothTask(void *pvParameters)
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
            if (btState == SLAVE)
            {
                if (currentBluetoothMode != MODE_RECEIVER)
                {
                    currentBluetoothMode = MODE_RECEIVER;
                }
                LinacQuatData dataToSend = {localBnoData.linearAccel, localBnoData.orientation};
                sendStruct(SerialBT, LinacQuatData_ID, &dataToSend, sizeof(LinacQuatData));
            }
            if (SerialBT.available())
            {
                receiveStruct(SerialBT);
            }
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
                    currentBluetoothMode = MODE_RECEIVER;
                }
                else
                {
                    delay(1000);
                    if (SerialBT.connected())
                    {
                        Serial.println("Connected Successfully!");
                        currentBluetoothMode = MODE_RECEIVER;
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
        vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust delay as needed
    }
}