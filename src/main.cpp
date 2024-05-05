#include <main.h>

bool isBTMaster = 0;

void setup()
{
  Serial.begin(115200);

  SerialBT.begin(bluetoothName, false);

  // Create the button task
  xTaskCreate(buttonTask, "ButtonTask", 4096, NULL, 1, &buttonTaskHandle);
  xTaskCreate(renderTask, "ButtonTask", 4096, NULL, 1, &renderTaskHandle);
  xTaskCreate(sensorTask, "ButtonTask", 4096, NULL, 1, &sensorTaskHandle);
  xTaskCreate(bluetoothTask, "ButtonTask", 4096, &SerialBT, 1, &bluetoothTaskHandle);
}

void loop()
{
  vTaskDelete(NULL);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}
