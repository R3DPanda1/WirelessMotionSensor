#include <main.h>

bool isBTMaster = 0;

void setup()
{
  Serial.begin(115200);

  SerialBT.begin(bluetoothName, false);

  // Create the button task
  xTaskCreate(buttonTask, "ButtonTask", 4096, NULL, 1, &buttonTaskHandle);
  xTaskCreate(renderTask, "RenderTask", 4096, NULL, 1, &renderTaskHandle);
  xTaskCreate(sensorTask, "SensorTask", 4096, NULL, 1, &sensorTaskHandle);
  xTaskCreate(bluetoothRXTask, "BluetoothRXTask", 4096, &SerialBT, 1, &bluetoothRXTaskHandle);
  xTaskCreate(bluetoothTXTask, "BluetoothTXTask", 4096, &SerialBT, 1, &bluetoothTXTaskHandle);
}

void loop()
{
  vTaskDelete(NULL);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}
