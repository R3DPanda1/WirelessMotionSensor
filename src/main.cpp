#include <main.h>

void setup()
{
  Serial.begin(115200);
  // Create the button task
  xTaskCreate(sensorTask, "SensorTask", 2048, NULL, 2, &sensorTaskHandle);  
  xTaskCreate(bluetoothTXTask, "BluetoothTXTask", 4096, &SerialBT, 2, &bluetoothTXTaskHandle);
  xTaskCreate(bluetoothRXTask, "BluetoothRXTask", 2048, &SerialBT, 3, &bluetoothRXTaskHandle);
  xTaskCreate(buttonTask, "ButtonTask", 1024, NULL, 1, &buttonTaskHandle);
  xTaskCreate(sdCardTask, "SDCardTask", 4096, NULL, 1, &sdCardTaskHandle);
  xTaskCreate(renderTask, "RenderTask", 4096, NULL, 1, &renderTaskHandle);
}

void loop()
{
  vTaskDelete(NULL);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}