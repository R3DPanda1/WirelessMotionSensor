#include <main.h>

// 3D Models
Vertex cube_vertices[] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}};
Index cube_indices[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
Vertex device_vertices[] = {{-3.5, 2, 1}, {3.5, 2, 1}, {3.5, -2, 1}, {-3.5, -2, 1}, {-3.5, 2, -1}, {3.5, 2, -1}, {3.5, -2, -1}, {-3.5, -2, -1}, {1.5, 1, -1}, {-1.5, 1, -1}, {-1.5, -1, -1}, {1.5, -1, -1}};
Index device_indices[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}, {8, 9}, {9, 10}, {10, 11}, {11, 8}};

bool isBTMaster = 0;

String bluetoothName = "WirelessMotionSensor"; // Bluetooth name of the devices

Model cubeModel;
Model deviceModel;

void setup()
{
  Serial.begin(115200);
  cubeModel = createModel(cube_vertices, cube_indices, sizeof(cube_indices) / sizeof(cube_indices[0]));
  deviceModel = createModel(device_vertices, device_indices, sizeof(device_indices) / sizeof(device_indices[0]));

  if (!display.begin(0x3C, true))
  {
    Serial.println(F("SH110X allocation failed"));
    for (;;)
      ;
  }

  if (!bno.begin())
  {
    Serial.println(F("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!"));
    while (1)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);             // Set text size to normal.
  display.setTextColor(SH110X_WHITE); // Set color to white.

  SerialBT.begin(bluetoothName, false);

  // Create the button task
  xTaskCreate(buttonTask, "ButtonTask", 4096, NULL, 1, &buttonTaskHandle);
  xTaskCreate(bluetoothTask, "ButtonTask", 4096, &SerialBT, 1, &bluetoothTaskHandle);
}

void loop()
{
  vTaskDelete(NULL);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}
