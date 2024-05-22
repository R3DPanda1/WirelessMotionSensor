#include <main.h>

bool isBTMaster = 0;

void setup()
{
  Serial.begin(115200);

  SerialBT.begin(bluetoothName, false);

  // Create the button task
  xTaskCreate(buttonTask, "ButtonTask", 1024, NULL, 1, &buttonTaskHandle);
  xTaskCreate(renderTask, "RenderTask", 4096, NULL, 1, &renderTaskHandle);
  xTaskCreate(sensorTask, "SensorTask", 2048, NULL, 2, &sensorTaskHandle);
  xTaskCreate(bluetoothRXTask, "BluetoothRXTask", 2048, &SerialBT, 3, &bluetoothRXTaskHandle);
  xTaskCreate(bluetoothTXTask, "BluetoothTXTask", 4096, &SerialBT, 2, &bluetoothTXTaskHandle);
  xTaskCreate(sdCardTask, "SDTask", 4096, NULL, 1, &sdCardTaskHandle);
}

void loop()
{
  vTaskDelete(NULL);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}