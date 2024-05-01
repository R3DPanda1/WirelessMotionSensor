#include <rendering.h>

void drawRotatedObj(Adafruit_SH1106G& display, Model model, float objSize, float objOffset_x, float objOffset_y, double qW, double qX, double qY, double qZ) {

  float cameraDistance = 100;  // controls perspective projection amount

  for (int i = 0; i < model.numIndices; i++) {
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

    // Rotate vertices using quaternions
    rotatePoint(x1, y1, z1, qW, qX, qY, qZ);
    rotatePoint(x2, y2, z2, qW, qX, qY, qZ);

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

// Function to multiply two quaternions
Quat multiplyQuaternions(Quat quat1, Quat quat2) {
  Quat result;
  result.qW = quat1.qW * quat2.qW - quat1.qX * quat2.qX - quat1.qY * quat2.qY - quat1.qZ * quat2.qZ;
  result.qX = quat1.qW * quat2.qX + quat1.qX * quat2.qW + quat1.qY * quat2.qZ - quat1.qZ * quat2.qY;
  result.qY = quat1.qW * quat2.qY - quat1.qX * quat2.qZ + quat1.qY * quat2.qW + quat1.qZ * quat2.qX;
  result.qZ = quat1.qW * quat2.qZ + quat1.qX * quat2.qY - quat1.qY * quat2.qX + quat1.qZ * quat2.qW;
  return result;
}

void rotatePoint(float& x, float& y, float& z, double& qW, double& qX, double& qY, double& qZ) {
  // Convert the point to a quaternion
  Quat p = { 0, x, y, z };
  // Create a quaternion representing the rotation
  Quat q = { qW, qX, qY, qZ };
  // Create the conjugate of the rotation quaternion
  Quat qConj = { qW, -qX, -qY, -qZ };

  // Multiply the rotation quaternion by the point quaternion (q * p * qConj)
  Quat result = multiplyQuaternions(q, p);
  Quat finalResult = multiplyQuaternions(result, qConj);

  // Update the point coordinates
  x = finalResult.qX;
  y = finalResult.qY;
  z = finalResult.qZ;
}

// Function to create a model
Model createModel(Vertex* vertices, Index* indices, uint8_t numIndices) {
  Model model;
  model.vertices = vertices;
  model.indices = indices;
  model.numIndices = numIndices;
  return model;
}

void drawLinacQuat(Adafruit_SH1106G& display, uint8_t x, uint8_t y, LinacQuatData data) {
  display.setCursor(x, y);
  display.print("X: ");
  display.println(data.x, 4);
  display.print("Y: ");
  display.println(data.y, 4);
  display.print("Z: ");
  display.println(data.z, 4);
  display.print("qW: ");
  display.println(data.qW, 4);
  display.print("qX: ");
  display.println(data.qX, 4);
  display.print("qY: ");
  display.println(data.qY, 4);
  display.print("qZ: ");
  display.println(data.qZ, 4);
}