#ifndef RENDERING_H
#define RENDERING_H

#include <Arduino.h>
#include <Adafruit_SH110X.h>
//#include <Adafruit_SSD1306.h>
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

const uint8_t btSprite[] PROGMEM = {
    0b00111100,
    0b01101110,
    0b11100111,
    0b11101011,
    0b10101101,
    0b11001011,
    0b11100111,
    0b11001011,
    0b10101101,
    0b11101011,
    0b11100111,
    0b01101110,
    0b00111100};

// 'battery', 13x7px
#define batterySprite_W 13
#define batterySprite_H 7
const unsigned char batterySprite[] PROGMEM = {
    0xff, 0xf0, 0x80, 0x10, 0x80, 0x18, 0x80, 0x18, 0x80, 0x18, 0x80, 0x10, 0xff, 0xf0};

const uint8_t sdFailedSprite[] PROGMEM = {
    0b11111110,
    0b10101010,
    0b10101010,
    0b10101011,
    0b11111111,
    0b10111010,
    0b10010011,
    0b11000111,
    0b11000111,
    0b10010011,
    0b10111011,
    0b11111111};

const uint8_t sdSprite[] PROGMEM = {
    0b11111110,
    0b10101010,
    0b10101010,
    0b10101011,
    0b11111111,
    0b11111110,
    0b10010011,
    0b10110101,
    0b10010101,
    0b11010101,
    0b10010011,
    0b11111111};

const uint8_t triAxisSprite[] PROGMEM = {
    0b00000010, 0b00000111, 0b00000010, 0b10001010, 0b01010010, 0b00100010, 0b01010010, 0b10001010,
    0b00000010, 0b00001111, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000000, 0b00000010, 0b00000111, 0b00000010, 0b10001010,
    0b01010010, 0b00100010, 0b00100010, 0b00100010, 0b00000010, 0b00001111, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000000,
    0b00000010, 0b00000111, 0b00000010, 0b11111010, 0b00010010, 0b00100010, 0b01000010, 0b11111010,
    0b00000010, 0b00001111, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000000};

// 'temp', 19x52px
#define thermometSprite_W 19
#define thermometSprite_H 52
const unsigned char thermometSprite[] PROGMEM = {
    0x03, 0xc0, 0x00, 0x04, 0x20, 0x00, 0x08, 0x10, 0x00, 0x08, 0x13, 0xc0, 0x08, 0x10, 0x00, 0x08,
    0x12, 0x00, 0x08, 0x10, 0x00, 0x08, 0x12, 0x00, 0x08, 0x10, 0x00, 0x08, 0x12, 0x00, 0x08, 0x10,
    0x00, 0x08, 0x13, 0xc0, 0x08, 0x10, 0x00, 0x08, 0x12, 0x00, 0x08, 0x10, 0x00, 0x08, 0x12, 0x00,
    0x08, 0x10, 0x00, 0x08, 0x12, 0x00, 0x08, 0x10, 0x00, 0x08, 0x13, 0xc0, 0x08, 0x10, 0x00, 0x08,
    0x12, 0x00, 0x08, 0x10, 0x00, 0x08, 0x12, 0x00, 0x08, 0x10, 0x00, 0x08, 0x12, 0x00, 0x08, 0x10,
    0x00, 0x08, 0x13, 0xc0, 0x08, 0x10, 0x00, 0x08, 0x12, 0x00, 0x08, 0x10, 0x00, 0x08, 0x12, 0x00,
    0x08, 0x10, 0x00, 0x08, 0x12, 0x00, 0x08, 0x10, 0x00, 0x08, 0x13, 0xc0, 0x08, 0x10, 0x00, 0x08,
    0x10, 0x00, 0x08, 0x10, 0x00, 0x08, 0x10, 0x00, 0x10, 0x08, 0x00, 0x23, 0xc4, 0x00, 0x47, 0xe4,
    0x00, 0x4f, 0xf2, 0x00, 0x4f, 0xf2, 0x00, 0x4f, 0xf2, 0x00, 0x4f, 0xf2, 0x00, 0x47, 0xe2, 0x00,
    0x23, 0xc4, 0x00, 0x10, 0x08, 0x00, 0x08, 0x10, 0x00, 0x07, 0xe0, 0x00};

// 'sync', 41x30px
#define syncSprite_W 41
#define syncSprite_H 30
const unsigned char syncSprite[] PROGMEM = {
    0x03, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x04, 0x03, 0xff, 0xe6,
    0x10, 0x00, 0x04, 0x02, 0x00, 0x26, 0x10, 0x00, 0x04, 0x02, 0x00, 0x20, 0x10, 0x00, 0x04, 0x02,
    0x00, 0x20, 0x10, 0x00, 0x04, 0x02, 0x00, 0x26, 0x10, 0x00, 0x04, 0x02, 0x00, 0x26, 0x10, 0x00,
    0x04, 0x02, 0x00, 0x20, 0x10, 0x00, 0x04, 0x03, 0xff, 0xe0, 0x10, 0x00, 0x04, 0x30, 0x00, 0x06,
    0x10, 0x00, 0x84, 0x30, 0x00, 0x06, 0x10, 0x80, 0x64, 0x00, 0x00, 0x00, 0x13, 0x00, 0x33, 0xff,
    0xff, 0xff, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x03, 0x80,
    0x03, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x34, 0x00, 0x00, 0x00, 0x16, 0x00, 0x44, 0x03, 0xff, 0xe6,
    0x11, 0x00, 0x84, 0x02, 0x00, 0x26, 0x10, 0x80, 0x04, 0x02, 0x00, 0x20, 0x10, 0x00, 0x04, 0x02,
    0x00, 0x20, 0x10, 0x00, 0x04, 0x02, 0x00, 0x26, 0x10, 0x00, 0x04, 0x02, 0x00, 0x26, 0x10, 0x00,
    0x04, 0x02, 0x00, 0x20, 0x10, 0x00, 0x04, 0x03, 0xff, 0xe0, 0x10, 0x00, 0x04, 0x30, 0x00, 0x06,
    0x10, 0x00, 0x04, 0x30, 0x00, 0x06, 0x10, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0xff,
    0xff, 0xff, 0xe0, 0x00};

void drawRotatedObj(Adafruit_SH1106G &display, Model model, float objSize, float objOffset_x, float objOffset_y, imu::Quaternion quat);
void drawVectorGraph(Adafruit_SH1106G &display, imu::Vector<3> accel, int x_position, const int graph_x_size, int8_t *xVals, int8_t *yVals, int8_t *zVals);
int scaleLogarithmically(float val, float sensitivity);
void rotatePoint(float &x, float &y, float &z, imu::Quaternion quat);
Model createModel(Vertex *vertices, Index *indices, uint8_t numIndices);

void renderTask(void *pvParameters);
extern TaskHandle_t renderTaskHandle;

extern Adafruit_SH1106G display;

#endif // RENDERING_H