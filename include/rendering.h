#ifndef RENDERING_H
#define RENDERING_H

#include <Arduino.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_GFX.h>
#include <bno055.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define a struct for a 3D vertex
struct Vertex {
  float x;
  float y;
  float z;
};

// Define a struct for an index pair
struct Index {
  int v1;
  int v2;
};

// Define a struct for a 3D model
struct Model {
  Vertex* vertices;
  Index* indices;
  uint8_t numIndices;
};

void drawRotatedObj(Adafruit_SH1106G& display, Model model, float objSize, float objOffset_x, float objOffset_y, double qW, double qX, double qY, double qZ);
void rotatePoint(float& x, float& y, float& z, double& qW, double& qX, double& qY, double& qZ);
void drawLinacQuat(Adafruit_SH1106G& display, uint8_t x, uint8_t y, LinacQuatData data);
Model createModel(Vertex* vertices, Index* indices, uint8_t numIndices);

#endif // RENDERING_H