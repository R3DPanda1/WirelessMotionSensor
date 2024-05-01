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

void buttonTask(void *pvParameters)
{

  const int buttonPins[] = {REC_SW_PIN, BT_SW_PIN, MODE_SW_PIN, CLK_SYNC_SW_PIN};

  enum buttonNames
  {
    REC_SW = 1,
    BT_SW = 2,
    MODE_SW = 3,
    CLK_SYNC_SW = 4
  };

  int lastButtonStates[] = {HIGH, HIGH, HIGH, HIGH};
  long lastDebounceTimes[] = {0, 0, 0, 0};
  long debounceDelay = 100;

  int buttonPressed = 0;

  // initialize buttons
  for (int i = 0; i < 4; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // loop
  for (;;)
  {
    for (int i = 0; i < 4; i++)
    {
      int buttonState = digitalRead(buttonPins[i]);

      // check for state change from HIGH to LOW
      if (buttonState == LOW && lastButtonStates[i] == HIGH && millis() - lastDebounceTimes[i] > debounceDelay)
      {
        // button state changed from HIGH to LOW
        lastDebounceTimes[i] = millis();

        buttonPressed = i + 1;
      }

      lastButtonStates[i] = buttonState;
    }

    switch (buttonPressed)
    {
    case REC_SW:
      // Statement(s)
      break;
    case BT_SW:
      // Statement(s)
      break;
    case MODE_SW:
      // Statement(s)
      break;
    case CLK_SYNC_SW:
      // Statement(s)
      break;
    default:
      // no button pressed
      break;
    }

    // Yield to other tasks
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}