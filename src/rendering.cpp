#include <rendering.h>

// 3D Models
Vertex cube_vertices[] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}};
Index cube_indices[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
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
    Serial.println(F("SH110X allocation failed"));
    vTaskDelete(NULL);
  }

  display.clearDisplay();
  display.setTextSize(1);             // Set text size to normal.
  display.setTextColor(SH110X_WHITE); // Set color to white.

  cubeModel = createModel(cube_vertices, cube_indices, sizeof(cube_indices) / sizeof(cube_indices[0]));
  deviceModel = createModel(device_vertices, device_indices, sizeof(device_indices) / sizeof(device_indices[0]));

  displayNotificationQueue = xQueueCreate(3, sizeof(char *));
  char *notification;

  for (;;)
  {
    if (uxQueueMessagesWaiting(displayNotificationQueue) > 0)
    {
      if (xQueueReceive(displayNotificationQueue, &notification, portMAX_DELAY) == pdTRUE)
      {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(notification);
        display.display();
        free(notification);             // Free the memory after displaying
        vTaskDelay(pdMS_TO_TICKS(500)); // Display each message for 500 ms
      }
    }
    else
    {
      display.clearDisplay();
      LinacQuatData renderData = {localBnoData.linearAccel, localBnoData.orientation};
      // drawLinacQuat(display, 0, 0, renderData);
      imu::Quaternion cubeAdjustedQuat = {renderData.orientation.w(), -renderData.orientation.x(), renderData.orientation.y(), renderData.orientation.z()};
      drawRotatedObj(display, cubeModel, 17, SCREEN_WIDTH / 4 * 1, SCREEN_HEIGHT / 2, cubeAdjustedQuat);
      if (currentBluetoothMode == MODE_CONNECTED)
      {
        renderData.linearAccel = remoteBnoData.linearAccel;
        renderData.orientation = remoteBnoData.orientation;
        // drawLinacQuat(display, 0, 0, renderData);
        cubeAdjustedQuat = {renderData.orientation.w(), -renderData.orientation.x(), renderData.orientation.y(), renderData.orientation.z()};
        drawRotatedObj(display, cubeModel, 17, SCREEN_WIDTH / 4 * 3, SCREEN_HEIGHT / 2, cubeAdjustedQuat);
      }
      display.display();
      delay(10);
    }
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