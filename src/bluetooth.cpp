#include "bluetooth.h"
#include "modes.h"

TaskHandle_t bluetoothTaskHandle = NULL;

BluetoothConnectionState btState = UNPAIRED;

void sendBT(BluetoothSerial &SerialBT, const void *data, size_t dataSize)
{
    SerialBT.write((const uint8_t *)data, dataSize);
}

void receiveBT(BluetoothSerial &SerialBT, void *data, size_t dataSize)
{
    SerialBT.readBytes((char *)data, dataSize);
}

void bluetoothTask(void *pvParameters)
{
    BluetoothSerial *SerialBT = (BluetoothSerial *)pvParameters;

    // Periodically check if the button is pressed
    for (;;)
    {
        if (SerialBT->connected())
        {
        }
        else
        {
            if (currentBluetoothMode == MODE_CONNECT)
            {
                // Change bluetooth mode to master
                SerialBT->end();
                SerialBT->begin(bluetoothName, true);
                btState = MASTER;

                // Try to connect to a device
                if (SerialBT->connect(bluetoothName))
                {
                    Serial.println("Connected Successfully!");
                    currentBluetoothMode = MODE_RECEIVER;
                }
                else
                {
                    delay(1000);
                    if (SerialBT->connected())
                    {
                        Serial.println("Connected Successfully!");
                        currentBluetoothMode = MODE_RECEIVER;
                    }
                    else
                    {
                        Serial.println("Couldn't connect!");
                        SerialBT->disconnect();
                        // Change bluetooth mode to slave
                        SerialBT->end();
                        SerialBT->begin(bluetoothName, false);
                        btState = SLAVE;
                        currentBluetoothMode = MODE_DISCONNECTED;
                    }
                }

                Serial.printf("Connecting as the master, make sure a \"%s\" device is on!\n", bluetoothName.c_str());
                Serial.println("Connected Successfully!");
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust delay as needed
    }
}