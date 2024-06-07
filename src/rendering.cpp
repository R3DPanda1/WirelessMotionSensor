#include <rendering.h>

// 3D Models
Vertex cube_vertices[] = {{-5, -5, -5}, {5, -5, -5}, {5, 5, -5}, {-5, 5, -5}, {-5, -5, 5}, {5, -5, 5}, {5, 5, 5}, {-5, 5, 5}, {0, 0, 0}, {0, 0, -7}, {1, 0, -4}, {-1, 0, -4}, {0, -1, -4}, {0, 1, -4}, {0, -4, 0}, {0, -2.5, 0.5}, {0, -2.5, -0.5}, {-0.5, -2.5, 0}, {0.5, -2.5, 0}, {4, 0, 0}, {2.5, 0.5, 0}, {2.5, -0.5, 0}, {2.5, 0, -0.5}, {2.5, 0, 0.5}};
Index cube_indices[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}, {8, 9}, {9, 10}, {9, 11}, {9, 12}, {9, 13}, {8, 14}, {14, 15}, {14, 16}, {14, 17}, {14, 18}, {8, 19}, {19, 20}, {19, 21}, {19, 22}, {19, 23}};
// Vertex device_vertices[] = {{-3.5, 2, 1}, {3.5, 2, 1}, {3.5, -2, 1}, {-3.5, -2, 1}, {-3.5, 2, -1}, {3.5, 2, -1}, {3.5, -2, -1}, {-3.5, -2, -1}, {1.5, 1, -1}, {-1.5, 1, -1}, {-1.5, -1, -1}, {1.5, -1, -1}};
// Index device_indices[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}, {8, 9}, {9, 10}, {10, 11}, {11, 8}};

Model cubeModel;
Model deviceModel;

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

TaskHandle_t renderTaskHandle = NULL;

QueueHandle_t displayNotificationQueue;

void renderTask(void *pvParameters)
{
  if (!display.begin(0x3C, true))
  {
    Serial.println(F("SH110X initialization failed"));
    vTaskDelete(NULL);
  }

  display.clearDisplay();
  display.setTextSize(1);             // Set text size to normal.
  display.setTextColor(SH110X_WHITE); // Set color to white.

  cubeModel = createModel(cube_vertices, cube_indices, sizeof(cube_indices) / sizeof(cube_indices[0]));
  // deviceModel = createModel(device_vertices, device_indices, sizeof(device_indices) / sizeof(device_indices[0]));

  displayNotificationQueue = xQueueCreate(3, sizeof(char *));

  //pinMode(TEST_INT_PIN, INPUT_PULLUP);
  //attachInterrupt(TEST_INT_PIN, test_isr, FALLING);

  for (;;)
  {
    // unsigned long startTime = millis();
    // static unsigned long renderTime = 0;

    // copy IMU data
    IMU_Data localImuData;
    IMU_Data remoteImuData;
    if (xSemaphoreTake(localImuSemaphore, portMAX_DELAY) == pdTRUE)
    {
      localImuData = localImuDataGL;
      xSemaphoreGive(localImuSemaphore);
    }
    if (xSemaphoreTake(remoteImuSemaphore, portMAX_DELAY) == pdTRUE)
    {
      remoteImuData = remoteImuDataGL;
      xSemaphoreGive(remoteImuSemaphore);
    }

    // Receive a notification
    static int16_t x1, y1;
    static uint16_t w, h;
    static unsigned long drawNotificationUntil = 0;
    static char *notification;
    if (uxQueueMessagesWaiting(displayNotificationQueue) > 0 && millis() > drawNotificationUntil)
    {
      if (xQueueReceive(displayNotificationQueue, &notification, portMAX_DELAY) == pdTRUE)
      {
        display.setTextSize(1);
        // Calculate the width and height of the notification text
        display.getTextBounds(notification, 0, 0, &x1, &y1, &w, &h);

        // Display the notification
        drawNotificationUntil = millis() + 1000;
      }
    }

    display.clearDisplay();

    IMU_Data renderedBnoData = localImuData;
    if (currentBluetoothMode == MODE_CONNECTED)
    {
      renderedBnoData = remoteImuData;
    }

    imu::Quaternion rerotatedQuat = {1, 0, 0, 0};
    imu::Vector<3> euler;
    switch (currentOperationMode)
    {
    case MODE_FUSION:
    {
      if (currentBluetoothMode == MODE_CONNECTED)
      {
        // Some of the axes need inverting to achieve desired behaviour. These were found by trial and error
        imu::Quaternion quat1 = {localImuData.orientation.w(), -localImuData.orientation.x(), localImuData.orientation.y(), localImuData.orientation.z()};
        imu::Quaternion quat2 = {remoteImuData.orientation.w(), remoteImuData.orientation.x(), -remoteImuData.orientation.y(), -remoteImuData.orientation.z()};
        // Calculate orientation of remote device from the point of view of the local device
        rerotatedQuat = quat1 * quat2;
      }
      else
      {
        // Make a orientation quaternion to rotate the cube so the long axis faces north
        imu::Quaternion appliedRotation = {0.5, 0.5, -0.5, -0.5}; // calculated using https://quaternions.online/
        // Invert the orientation to achieve the effect of "still in space cube"
        rerotatedQuat = {renderedBnoData.orientation.w(), -renderedBnoData.orientation.x(), renderedBnoData.orientation.y(), renderedBnoData.orientation.z()};
        rerotatedQuat = rerotatedQuat * appliedRotation; // apply orientation to face Z axis to north
      }

      drawRotatedObj(display, cubeModel, 3.5, SCREEN_WIDTH / 4 * 1, SCREEN_HEIGHT / 2, rerotatedQuat);
      // Draw the cube model using the rerotated Quaternion
      const int graph_size = 50;
      static int8_t accelVals[3][graph_size] = {0};
      drawVectorGraph(display, renderedBnoData.linearAccel, SCREEN_WIDTH - graph_size, graph_size, accelVals[0], accelVals[1], accelVals[2]);
      break;
    }
    case MODE_LEVEL:
    {
      // Convert orientation to to yaw, pitch, roll
      euler = renderedBnoData.orientation.toEuler();

      // Euler angles are in radians, convert to degrees
      euler.toDegrees();

      // Calculate the tilt
      int tilt = sqrt(euler.z() * euler.z() + euler.y() * euler.y());

      // Calculate positions for spirit level circles
      int centerX = SCREEN_WIDTH / 2;
      int centerY = SCREEN_HEIGHT / 2;
      int radius = 30;
      int maxOffset = 90 + radius;

      // Calculate offsets based on roll and yaw
      int offsetX = map(euler.y(), 90, -90, -maxOffset, maxOffset);
      int offsetY = map(euler.z(), 90, -90, -maxOffset, maxOffset);
      int16_t x1, y1;
      uint16_t textWidth, textHeight;
      display.setTextSize(2);
      display.setTextColor(SH110X_INVERSE);

      if (euler.z() > 45) // roll is more than 45 degrees, start drawing leveling line
      {
        offsetX = map(euler.y(), 26, -26, -centerY, centerY); // not exact. 26 is chosen by trial and error
        offsetY = map(euler.z(), 0, 180, -SCREEN_HEIGHT, SCREEN_HEIGHT);
        // draw horizontal level quadrangle using triangles
        if (euler.y() < 80 && euler.y() > -80) // triangle render method breaks for values outside this range
        {
          display.fillTriangle(centerX, SCREEN_HEIGHT, 0, centerY - offsetX - offsetY, SCREEN_WIDTH, centerY + offsetX - offsetY, SH110X_WHITE);
          display.fillTriangle(centerX, SCREEN_HEIGHT, 0, centerY - offsetX - offsetY, 0, SCREEN_HEIGHT, SH110X_WHITE);
          display.fillTriangle(centerX, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, centerY + offsetX - offsetY, SH110X_WHITE);
        }
        display.getTextBounds(String((int)euler.y()), 0, 0, &x1, &y1, &textWidth, &textHeight);
        display.setCursor(centerX - textWidth / 2, centerY - textHeight / 2);
        display.print((int)euler.y());
      }
      else
      {
        // Draw spirit level indicator
        display.fillCircle(centerX + offsetX, centerY + offsetY, radius, SH110X_INVERSE);
        display.fillCircle(centerX - offsetX, centerY - offsetY, radius + 1, SH110X_INVERSE);
        display.getTextBounds(String(tilt), 0, 0, &x1, &y1, &textWidth, &textHeight);
        display.setCursor(centerX - textWidth / 2, centerY - textHeight / 2);
        display.print(tilt);
      }
      break;
    }
    case MODE_TEMP:
      display.setCursor(18, 0);
      display.print("Local");
      display.drawBitmap(13, 12, thermometSprite, thermometSprite_W, thermometSprite_H, SH110X_WHITE);
      display.fillRect(19, 14 + 39 - localImuData.temperature / 2, 4, localImuData.temperature / 2, SH110X_WHITE);
      display.setCursor(15 + thermometSprite_W, 13);
      display.setTextSize(2);
      display.print(localImuData.temperature);
      display.print("C");
      display.setTextSize(1);
      if (currentBluetoothMode == MODE_CONNECTED)
      {
        display.setTextSize(1);
        display.setCursor(57 + 18, 0);
        display.print("Remote");
        display.drawBitmap(57 + 13, 12, thermometSprite, thermometSprite_W, thermometSprite_H, SH110X_WHITE);
        display.fillRect(57 + 19, 14 + 39 - remoteImuData.temperature / 2, 4, remoteImuData.temperature / 2, SH110X_WHITE);
        display.setCursor(57 + 15 + thermometSprite_W, 13);
        display.setTextSize(2);
        display.print(remoteImuData.temperature);
        display.print("C");
        display.setTextSize(1);
      }
      // display.println(remoteBnoData.temperature);
      break;
    case MODE_RAW:
    {
      const int graph_size = 20;
      // Calculated value storage for graphs
      static int8_t accelVals[3][graph_size] = {0};
      static int8_t magVals[3][graph_size] = {0};
      static int8_t gyroVals[3][graph_size] = {0};
      // Draw graphs
      drawVectorGraph(display, renderedBnoData.accelerometer, 30, graph_size, accelVals[0], accelVals[1], accelVals[2]);
      drawVectorGraph(display, renderedBnoData.gyroscope, 45 + graph_size, graph_size, gyroVals[0], gyroVals[1], gyroVals[2]);
      drawVectorGraph(display, renderedBnoData.magnetometer, SCREEN_WIDTH - graph_size, graph_size, magVals[0], magVals[1], magVals[2]);
      break;
    }
    case MODE_CLK_SYNC:
      display.setTextSize(1);
      display.setCursor(5, 20);
      display.print("Tap devices to sync!");
      display.drawBitmap(8, 32, syncSprite, syncSprite_W, syncSprite_H, SH110X_WHITE);
      display.setCursor(55, 35);
      display.print("L:");
      display.print(syncedMillis());
      display.setCursor(55, 50);
      display.print("R:");
      display.print(remoteImuData.timestamp);
      break;
    }

    // Show notification
    const uint16_t padding = 3;
    if (millis() < drawNotificationUntil)
    {
      // Set cursor position to horizontally center the text
      display.setCursor((SCREEN_WIDTH - w) / 2, padding);
      // Draw rectangle around the text
      display.fillRect((SCREEN_WIDTH - w) / 2 - padding, 0, w + 2 * padding, h + 2 * padding, SH110X_BLACK);
      display.drawRect((SCREEN_WIDTH - w) / 2 - padding, 0, w + 2 * padding, h + 2 * padding, SH110X_WHITE);

      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.print(notification);
    }

    if (currentBluetoothMode == MODE_CONNECTED || (currentBluetoothMode == MODE_CONNECTING && TOGGLE_200MS_STATE))
    {
      renderedBnoData = remoteImuData;
      display.drawBitmap(0, 0, btSprite, 8, sizeof(btSprite), SH110X_INVERSE);
    }

    if (currentSdState == CONNECTED)
    {
      if (currentRecordingMode != RECORDING || TOGGLE_200MS_STATE)
      {
        display.drawBitmap(0, SCREEN_HEIGHT - sizeof(sdSprite), sdSprite, 8, sizeof(sdSprite), SH110X_INVERSE);
      }
    }
    else if (currentSdState == FAILED)
    {
      display.drawBitmap(0, SCREEN_HEIGHT - sizeof(sdFailedSprite), sdFailedSprite, 8, sizeof(sdSprite), SH110X_INVERSE);
    }

    // Draw battery level icon
    static int batt_level = 0;
    static int batt_pixels = 0;
    static unsigned long nextBatteryMesurement = 0;
    if (nextBatteryMesurement < millis())
    {
      batt_level = analogRead(LIPO_MONITOR_PIN);
      batt_pixels = map(batt_level, 1700, 2300, 0, 8);
      batt_pixels = constrain(batt_pixels, 0, 8);
      nextBatteryMesurement = millis() + 3000;
    }
    display.drawBitmap(SCREEN_WIDTH - 13, 0, batterySprite, batterySprite_W, batterySprite_H, SH110X_INVERSE);
    display.fillRect(SCREEN_WIDTH - 13 + 2, 2, batt_pixels, 3, SH110X_INVERSE);

    // Testing only
    /*
    //display.setCursor(30, 0);
    //display.print("INT at:");
    //display.print(test_timestamp);
    display.setCursor(30, 0);
    display.print("Sent:");
    display.print(sentSyncTimestamp);
    display.setCursor(20, 10);
    display.print("Received at:");
    display.print(reveivedSyncTimestampAt);
    // display.setCursor(30, 10);
    // display.print(renderTime);
    */

    display.display();
    // renderTime = millis() - startTime;
    if (currentOperationMode == MODE_CLK_SYNC)
      delay(5);
    else
      delay(30);
  }
}

int scaleLogarithmically(float val, float sensitivity)
{
  if (val == 0)
    return 0;                                       // Logarithm of zero is undefined
  float logValue = log(abs(val) + 1) * sensitivity; // Add 1 to avoid log(0)
  return (int)val;
}

void drawVectorGraph(Adafruit_SH1106G &display, imu::Vector<3> accel, int x_position, const int graph_x_size, int8_t *xVals, int8_t *yVals, int8_t *zVals)
{
  int y_scale = 4;

  // Shift all data left by one position
  memmove(xVals, xVals + 1, graph_x_size - 1);
  memmove(yVals, yVals + 1, graph_x_size - 1);
  memmove(zVals, zVals + 1, graph_x_size - 1);

  // Add new scaled and shifted data to the end
  xVals[graph_x_size - 1] = -1 * constrain(scaleLogarithmically(accel.x(), y_scale), -10, 10);
  yVals[graph_x_size - 1] = -1 * constrain(scaleLogarithmically(accel.y(), y_scale), -10, 10);
  zVals[graph_x_size - 1] = -1 * constrain(scaleLogarithmically(accel.z(), y_scale), -10, 10);

  // SCREEN_WIDTH - graph_x_size - 8
  display.drawBitmap(x_position - 7, 1, triAxisSprite, 8, sizeof(triAxisSprite), SH110X_WHITE);

  int accel_nums[3] = {
      (int)(accel.x() * 10),
      (int)(accel.y() * 10),
      (int)(accel.z() * 10)};

  int len;
  for (int i = 0; i < 3; i++)
  {
    len = snprintf(nullptr, 0, "%d", accel_nums[i]);
    display.setCursor(x_position - 2 - len * 6, SCREEN_HEIGHT / 6 * (2 * i + 1) + 2);
    display.print(accel_nums[i]);
  }

  for (int i = 0; i < graph_x_size; i++)
  {
    // Drawing lines to make graphs
    display.drawFastVLine(x_position + graph_x_size - 1 - i, SCREEN_HEIGHT / 6 * 1, xVals[i], SH110X_WHITE);
    display.drawFastVLine(x_position + graph_x_size - 1 - i, SCREEN_HEIGHT / 6 * 3, yVals[i], SH110X_WHITE);
    display.drawFastVLine(x_position + graph_x_size - 1 - i, SCREEN_HEIGHT / 6 * 5, zVals[i], SH110X_WHITE);
  }
}

void drawRotatedObj(Adafruit_SH1106G &display, Model model, float objSize, float objOffset_x, float objOffset_y, imu::Quaternion quat)
{

  float cameraDistance = 100; // controls perspective projection amount

  for (int i = 0; i < model.numIndices; i++)
  {
    Index index = model.indices[i];
    Vertex vertex1 = model.vertices[index.v1];
    Vertex vertex2 = model.vertices[index.v2];

    // Apply scale to line coordinates
    float x1 = vertex1.x * objSize;
    float y1 = vertex1.y * objSize;
    float z1 = vertex1.z * objSize;
    float x2 = vertex2.x * objSize;
    float y2 = vertex2.y * objSize;
    float z2 = vertex2.z * objSize;

    // Rotate vertices using quaternion
    rotatePoint(x1, y1, z1, quat);
    rotatePoint(x2, y2, z2, quat);

    // Apply perspective projection
    float scaleFactor1 = cameraDistance / (cameraDistance + z1);
    float scaleFactor2 = cameraDistance / (cameraDistance + z2);

    // Translate and draw lines on the OLED display
    float tx1 = x1 * scaleFactor1 + objOffset_x;
    float ty1 = y1 * scaleFactor1 + objOffset_y;
    float tx2 = x2 * scaleFactor2 + objOffset_x;
    float ty2 = y2 * scaleFactor2 + objOffset_y;

    display.drawLine(tx1, ty1, tx2, ty2, SH110X_WHITE);
  }
}

void rotatePoint(float &x, float &y, float &z, imu::Quaternion quat)
{
  // Convert the point to a quaternion
  imu::Quaternion p = {0, x, y, z};

  // Multiply the orientation imu::Quaternion by the point imu::Quaternion (q * p * qConj)
  imu::Quaternion finalResult = quat * p * quat.conjugate();

  // Update the point coordinates
  x = finalResult.x();
  y = finalResult.y();
  z = finalResult.z();
}

// Function to create a model
Model createModel(Vertex *vertices, Index *indices, uint8_t numIndices)
{
  Model model;
  model.vertices = vertices;
  model.indices = indices;
  model.numIndices = numIndices;
  return model;
}