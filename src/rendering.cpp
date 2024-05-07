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
    if (uxQueueMessagesWaiting(displayNotificationQueue) > 0 && millis() > drawNotificationUntil)
    {
      if (xQueueReceive(displayNotificationQueue, &notification, portMAX_DELAY) == pdTRUE)
      {
        // Calculate the width and height of the notification text
        display.getTextBounds(notification, 0, 0, &x1, &y1, &w, &h);

        // Display the notification
        display.println(notification);
        // display.display();
        drawNotificationUntil = millis() + 1000;
      }
    }

    display.clearDisplay();

    // drawLinacQuat(display, 0, 0, renderData);
    imu::Quaternion cubeAdjustedQuat = {localBnoData.orientation.w(), -localBnoData.orientation.x(), localBnoData.orientation.y(), localBnoData.orientation.z()};
    drawRotatedObj(display, cubeModel, 3.5, SCREEN_WIDTH / 4 * 1, SCREEN_HEIGHT / 2, cubeAdjustedQuat);
    drawAccelGraph(display, localBnoData.linearAccel);
    if (currentBluetoothMode == MODE_CONNECTED)
    {
      // drawLinacQuat(display, 0, 0, renderData);
      cubeAdjustedQuat = {remoteBnoData.orientation.w(), -remoteBnoData.orientation.x(), remoteBnoData.orientation.y(), remoteBnoData.orientation.z()};
      drawRotatedObj(display, cubeModel, 3.5, SCREEN_WIDTH / 4 * 3, SCREEN_HEIGHT / 2, cubeAdjustedQuat);
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
    delay(10);
  }
}

int scaleLogarithmically(float val, float sensitivity)
{
  if (val == 0)
    return 0;                                              // Logarithm of zero is undefined, so handle this case separately.
  float logValue = log(abs(val) + 1) * sensitivity;      // Add 1 to avoid log(0), multiply by sensitivity to scale the result.
  return int((val > 0 ? 1 : -1) * min(logValue, 31.0f)); // Apply sign, constrain to -31 to 31.
}

void drawAccelGraph(Adafruit_SH1106G &display, imu::Vector<3> accel)
{
  const int graph_x_size = 50;
  static int8_t xVals[graph_x_size], yVals[graph_x_size], zVals[graph_x_size];
  int y_scale = 4;

  // Shift all data left by one position
  memmove(xVals, xVals + 1, graph_x_size-1);
  memmove(yVals, yVals + 1, graph_x_size-1);
  memmove(zVals, zVals + 1, graph_x_size-1);

  // Add new scaled and shifted data to the end
  xVals[graph_x_size-1] = constrain(scaleLogarithmically(accel.x(), y_scale), -10, 10);
  yVals[graph_x_size-1] = constrain(scaleLogarithmically(accel.y(), y_scale), -10, 10);
  zVals[graph_x_size-1] = constrain(scaleLogarithmically(accel.z(), y_scale), -10, 10);

  for (int i = 0; i < graph_x_size; i++)
  {
    // Drawing lines to make graphs
    display.drawFastVLine(SCREEN_WIDTH - 1 - i, SCREEN_HEIGHT / 6 * 1, xVals[i], SH110X_WHITE);
    display.drawFastVLine(SCREEN_WIDTH - 1 - i, SCREEN_HEIGHT / 6 * 3, yVals[i], SH110X_WHITE);
    display.drawFastVLine(SCREEN_WIDTH - 1 - i, SCREEN_HEIGHT / 6 * 5, zVals[i], SH110X_WHITE);
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

void drawLinacQuat(Adafruit_SH1106G &display, uint8_t x, uint8_t y, LinacQuatData data)
{
  display.setCursor(x, y);
  display.print("X: ");
  display.println(data.linearAccel.x(), 4);
  display.print("Y: ");
  display.println(data.linearAccel.y(), 4);
  display.print("Z: ");
  display.println(data.linearAccel.z(), 4);
  display.print("qW: ");
  display.println(data.orientation.w(), 4);
  display.print("qX: ");
  display.println(data.orientation.x(), 4);
  display.print("qY: ");
  display.println(data.orientation.y(), 4);
  display.print("qZ: ");
  display.println(data.orientation.z(), 4);
  display.print("BT-Mode: ");
  display.println(currentBluetoothMode, 4);
}