#ifndef RENDERING_H
#define RENDERING_H

#include <Arduino.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_GFX.h>
#include "commons.h"

#include <sstream>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define a struct for a 3D vertex
struct Vertex
{
  float x, y, z;
};

// Define a struct for an index pair
struct Index
{
  int v1, v2;
};

// Define a struct for a 3D model
struct Model
{
  Vertex *vertices;
  Index *indices;
  uint8_t numIndices;
};

const uint8_t btSprite[] = {
    0b00010000,
    0b00011000,
    0b00010100,
    0b00010010,
    0b10010001,
    0b01010010,
    0b00110100,
    0b00011000,
    0b00110100,
    0b01010010,
    0b10010001,
    0b00010010,
    0b00010100,
    0b00011000,
    0b00010000};

const uint8_t triAxisSprite[] = {
    0b00000010, 0b00000111, 0b00000010, 0b10001010, 0b01010010, 0b00100010, 0b01010010, 0b10001010,
    0b00000010, 0b00001111, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000000, 0b00000010, 0b00000111, 0b00000010, 0b10001010,
    0b01010010, 0b00100010, 0b00100010, 0b00100010, 0b00000010, 0b00001111, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000000,
    0b00000010, 0b00000111, 0b00000010, 0b11111010, 0b00010010, 0b00100010, 0b01000010, 0b11111010,
    0b00000010, 0b00001111, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000000};

void drawRotatedObj(Adafruit_SH1106G &display, Model model, float objSize, float objOffset_x, float objOffset_y, imu::Quaternion quat);
void drawAccelGraph(Adafruit_SH1106G &display, imu::Vector<3> accel);
int scaleLogarithmically(float val, float sensitivity);
void rotatePoint(float &x, float &y, float &z, imu::Quaternion quat);
Model createModel(Vertex *vertices, Index *indices, uint8_t numIndices);

void renderTask(void *pvParameters);
extern TaskHandle_t renderTaskHandle;

extern Adafruit_SH1106G display;

#endif // RENDERING_H