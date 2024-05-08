#include <rendering.h>

// 3D Models
Vertex cube_vertices[] = {{-5, -5, -5}, {5, -5, -5}, {5, 5, -5}, {-5, 5, -5}, {-5, -5, 5}, {5, -5, 5}, {5, 5, 5}, {-5, 5, 5}, {0, 0, 3}, {0, 0, -3}, {1, 1, -1}, {-1, 1, -1}, {1, -1, -1}, {-1, -1, -1}};
Index cube_indices[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}, {8, 9}, {9, 10}, {9, 11}, {9, 12}, {9, 13}};
Vertex device_vertices[] = {{-3.5, 2, 1}, {3.5, 2, 1}, {3.5, -2, 1}, {-3.5, -2, 1}, {-3.5, 2, -1}, {3.5, 2, -1}, {3.5, -2, -1}, {-3.5, -2, -1}, {1.5, 1, -1}, {-1.5, 1, -1}, {-1.5, -1, -1}, {1.5, -1, -1}};
Index device_indices[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}, {8, 9}, {9, 10}, {10, 11}, {11, 8}};

Model cubeModel;
Model deviceModel;

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
  deviceModel = createModel(device_vertices, device_indices, sizeof(device_indices) / sizeof(device_indices[0]));

  displayNotificationQueue = xQueueCreate(3, sizeof(char *));
  char *notification;
  int16_t x1, y1;
  uint16_t w, h;
  const uint16_t padding = 3;
  unsigned long drawNotificationUntil = 0;

  for (;;)
  {
    display.clearDisplay();

    BNO055Data renderedBnoData = localBnoData;
    if (currentBluetoothMode == MODE_CONNECTED)
    {
      renderedBnoData = remoteBnoData;
      display.drawBitmap(0, 0, btSprite, 8, sizeof(btSprite), SH110X_INVERSE);
    }
    if (SD_inserted == 1)
    {
      if (currentRecordingMode != SD_CARD || TOGGLE_200MS_STATE)
      {
        display.drawBitmap(0, SCREEN_HEIGHT - sizeof(sdSprite), sdSprite, 8, sizeof(sdSprite), SH110X_INVERSE);
      }
    }

    imu::Quaternion cubeAdjustedQuat;
    switch (currentOperationMode)
    {
    case MODE_LINACQUAD:
      cubeAdjustedQuat = {renderedBnoData.orientation.w(), -renderedBnoData.orientation.x(), renderedBnoData.orientation.y(), renderedBnoData.orientation.z()};
      drawRotatedObj(display, cubeModel, 3.5, SCREEN_WIDTH / 4 * 1, SCREEN_HEIGHT / 2, cubeAdjustedQuat);
      drawAccelGraph(display, renderedBnoData.linearAccel);
      break;
    case MODE_NONE:

      break;
    }

    if (millis() < drawNotificationUntil)
    {
      // Set cursor position to horizontally center the text
      display.setCursor((SCREEN_WIDTH - w) / 2, padding);
      // Draw rectangle around the text
      display.fillRect((SCREEN_WIDTH - w) / 2 - padding, 0, w + 2 * padding, h + 2 * padding, SH110X_BLACK);
      display.drawRect((SCREEN_WIDTH - w) / 2 - padding, 0, w + 2 * padding, h + 2 * padding, SH110X_WHITE);

      display.print(notification);
    }

    display.display();
    if (uxQueueMessagesWaiting(displayNotificationQueue) > 0 && millis() > drawNotificationUntil)
    {
      if (xQueueReceive(displayNotificationQueue, &notification, portMAX_DELAY) == pdTRUE)
      {
        // Calculate the width and height of the notification text
        display.getTextBounds(notification, 0, 0, &x1, &y1, &w, &h);

        // Display the notification
        display.println(notification);
        drawNotificationUntil = millis() + 1000;
      }
    }
    delay(10);
  }
}

int scaleLogarithmically(float val, float sensitivity)
{
  if (val == 0)
    return 0;                                       // Logarithm of zero is undefined
  float logValue = log(abs(val) + 1) * sensitivity; // Add 1 to avoid log(0)
  return (int)val;
}

void drawAccelGraph(Adafruit_SH1106G &display, imu::Vector<3> accel)
{
  const int graph_x_size = 50;
  static int8_t xVals[graph_x_size], yVals[graph_x_size], zVals[graph_x_size];
  int y_scale = 4;

  // Shift all data left by one position
  memmove(xVals, xVals + 1, graph_x_size - 1);
  memmove(yVals, yVals + 1, graph_x_size - 1);
  memmove(zVals, zVals + 1, graph_x_size - 1);

  // Add new scaled and shifted data to the end
  xVals[graph_x_size - 1] = constrain(scaleLogarithmically(accel.x(), y_scale), -10, 10);
  yVals[graph_x_size - 1] = constrain(scaleLogarithmically(accel.y(), y_scale), -10, 10);
  zVals[graph_x_size - 1] = constrain(scaleLogarithmically(accel.z(), y_scale), -10, 10);

  display.drawBitmap(SCREEN_WIDTH - graph_x_size - 8, 1, triAxisSprite, 8, sizeof(triAxisSprite), SH110X_WHITE);

  int accel_nums[3] = {
      (int)(accel.x() * 10),
      (int)(accel.y() * 10),
      (int)(accel.z() * 10)};

  int len;
  for (int i = 0; i < 3; i++)
  {
    len = snprintf(nullptr, 0, "%d", accel_nums[i]);
    display.setCursor(SCREEN_WIDTH - graph_x_size - 3 - len * 6, SCREEN_HEIGHT / 6 * (2 * i + 1) + 2);
    display.print(accel_nums[i]);
  }

  for (int i = 0; i < graph_x_size; i++)
  {
    // Drawing lines to make graphs
    display.drawFastVLine(SCREEN_WIDTH - 2 - i, SCREEN_HEIGHT / 6 * 1, xVals[i], SH110X_WHITE);
    display.drawFastVLine(SCREEN_WIDTH - 2 - i, SCREEN_HEIGHT / 6 * 3, yVals[i], SH110X_WHITE);
    display.drawFastVLine(SCREEN_WIDTH - 2 - i, SCREEN_HEIGHT / 6 * 5, zVals[i], SH110X_WHITE);
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

    // Apply size to line coordinates
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

  // Multiply the rotation imu::Quaternion by the point imu::Quaternion (q * p * qConj)
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