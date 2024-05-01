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
}

void loop()
{
  // --- Will be moved to seperate Tasks ---
  sensors_event_t linearAccelData;
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  imu::Quaternion quat = bno.getQuat();
  // Change around the axes of the cube to correct base position
  // imu::Quaternion adjustedQuat = imu::Quaternion(quat.w(), -quat.x(), quat.y(), quat.z());
  // quat = quat * rotationFix;

  LinacQuatData linacQuatReading;
  linacQuatReading.x = linearAccelData.acceleration.x;
  linacQuatReading.y = linearAccelData.acceleration.y;
  linacQuatReading.z = linearAccelData.acceleration.z;
  linacQuatReading.qW = quat.w();
  linacQuatReading.qX = quat.x();
  linacQuatReading.qY = quat.y();
  linacQuatReading.qZ = quat.z();

  display.clearDisplay();

  if (SerialBT.connected())
  {
    // Allocate buffer for packing values
    if (isBTMaster == 0)
    { // slave = sender
      display.setCursor(0, 0);
      display.println("Sending");
      drawLinacQuat(display, 0, 8, linacQuatReading);
      drawRotatedObj(display, cubeModel, 17, SCREEN_WIDTH / 4 * 3, SCREEN_HEIGHT / 2, linacQuatReading.qW, -linacQuatReading.qX, linacQuatReading.qY, linacQuatReading.qZ);
      sendBT(&linacQuatReading, sizeof(linacQuatReading));
      display.display();
      while (!SerialBT.available())
      { // wait for ping
      }
      // vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    else
    { // master = reciever
      if (SerialBT.available())
      {
        display.setCursor(0, 0);
        display.println("Recieving");
        LinacQuatData recievedLinacQuat;
        receiveBT(&recievedLinacQuat, sizeof(recievedLinacQuat));
        SerialBT.write(1); // ping
        Quat quat1 = {-linacQuatReading.qW, linacQuatReading.qX, linacQuatReading.qY, linacQuatReading.qZ};
        Quat quat2 = {recievedLinacQuat.qW, recievedLinacQuat.qX, recievedLinacQuat.qY, recievedLinacQuat.qZ};
        // Calculate relative orientation
        Quat relativeOrientation = multiplyQuaternions(quat2, quat1);
        drawLinacQuat(display, 0, 8, recievedLinacQuat);
        drawRotatedObj(display, deviceModel, 8, SCREEN_WIDTH / 4 * 3, SCREEN_HEIGHT / 2, relativeOrientation.qW, -relativeOrientation.qX, relativeOrientation.qY, -relativeOrientation.qZ);
        display.display();
      }
      // is reciever but didnt recieve anything
    }
  }
  else
  { // not bluetooth connected = standalone mode
    drawLinacQuat(display, 0, 0, linacQuatReading);
    drawRotatedObj(display, cubeModel, 17, SCREEN_WIDTH / 4 * 3, SCREEN_HEIGHT / 2, linacQuatReading.qW, -linacQuatReading.qX, linacQuatReading.qY, linacQuatReading.qZ);
    display.display();
  }
  vTaskDelay(10 / portTICK_PERIOD_MS);
}

/*
void buttonTask(void* pvParameters) {
  // Periodically check if the button is pressed
  bool connected;
  for (;;) {
    if (SerialBT.connected()) {

      // Evertlying in loop()

      //if (Serial.available()) {
      //  SerialBT.write(Serial.read());
      //}
      //if (SerialBT.available()) {
      //  Serial.write(SerialBT.read());dash
      //}
    } else {
      //isBTMaster = 0;
      if (digitalRead(BUTTON_PIN) == LOW) {
        SerialBT.end();
        SerialBT.begin(bluetoothName, true);
        Serial.printf("The device \"%s\" started in master mode, make sure slave BT device is on!\n", bluetoothName.c_str());
        isBTMaster = 1;
        // connect(address) is fast (up to 10 secs max), connect(bluetoothName) is slow (up to 30 secs max) as it needs
        // to resolve bluetoothName to address first, but it allows connecting to different devices with the same name.
        // Set CoreDebugLevel to Info to view devices Bluetooth address and device names
        connected = SerialBT.connect(bluetoothName);
        Serial.printf("Connecting to slave BT device named \"%s\"\n", bluetoothName.c_str());
        if (connected) {
          Serial.println("Connected Successfully!");
        } else {
          while (!SerialBT.connected(10000)) {
            Serial.println("Failed to connect. Make sure the remote device is available and in range, then restart the app.");
          }
        }
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Adjust delay as needed
  }
}*/

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

void sendBT(const void *data, size_t dataSize)
{
  SerialBT.write((const uint8_t *)data, dataSize);
}

void receiveBT(void *data, size_t dataSize)
{
  SerialBT.readBytes((char *)data, dataSize);
}