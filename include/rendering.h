#ifndef RENDERING_H
#define RENDERING_H

#include <Arduino.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_GFX.h>
#include "commons.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define a struct for a 3D vertex
struct Vertex {
  float x, y, z;
};

// Define a struct for an index pair
struct Index {
  int v1, v2;
};

// Define a struct for a 3D model
struct Model {
  Vertex* vertices;
  Index* indices;
  uint8_t numIndices;
};

void drawRotatedObj(Adafruit_SH1106G& display, Model model, float objSize, float objOffset_x, float objOffset_y, imu::Quaternion quat);
void drawAccelGraph(Adafruit_SH1106G &display, imu::Vector<3> accel);
void rotatePoint(float& x, float& y, float& z, imu::Quaternion quat);
void drawLinacQuat(Adafruit_SH1106G& display, uint8_t x, uint8_t y, LinacQuatData data);
Model createModel(Vertex* vertices, Index* indices, uint8_t numIndices);

void renderTask(void* pvParameters);
extern TaskHandle_t renderTaskHandle;

extern Adafruit_SH1106G display;

#endif // RENDERING_H