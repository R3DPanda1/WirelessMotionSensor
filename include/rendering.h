#ifndef RENDERING_H
#define RENDERING_H

#include <Arduino.h>
#include <Adafruit_SH110X.h>
//#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "commons.h"

#include <sstream>

#define SCREEN_WIDTH 128  // Width of the OLED display
#define SCREEN_HEIGHT 64  // Height of the OLED display

extern TaskHandle_t renderTaskHandle;  // Handle for the render task

extern Adafruit_SH1106G display;  // Display object

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
  Vertex *vertices;  // Array of vertices
  Index *indices;    // Array of indices
  uint8_t numIndices; // Number of indices
};

// Bluetooth icon sprite
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

// Battery icon sprite dimensions
#define batterySprite_W 13
#define batterySprite_H 7

// Battery icon sprite
const unsigned char batterySprite[] PROGMEM = {
    0xff, 0xf0, 0x80, 0x10, 0x80, 0x18, 0x80, 0x18, 0x80, 0x18, 0x80, 0x10, 0xff, 0xf0};

// SD card failed icon sprite
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

// SD card icon sprite
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

// Tri-axis icon sprite
const uint8_t triAxisSprite[] PROGMEM = {
    0b00000010, 0b00000111, 0b00000010, 0b10001010, 0b01010010, 0b00100010, 0b01010010, 0b10001010,
    0b00000010, 0b00001111, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000000, 0b00000010, 0b00000111, 0b00000010, 0b10001010,
    0b01010010, 0b00100010, 0b00100010, 0b00100010, 0b00000010, 0b00001111, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000000,
    0b00000010, 0b00000111, 0b00000010, 0b11111010, 0b00010010, 0b00100010, 0b01000010, 0b11111010,
    0b00000010, 0b00001111, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010,
    0b00000010, 0b00000010, 0b00000010, 0b00000000};

// Thermometer icon sprite dimensions
#define thermometSprite_W 19
#define thermometSprite_H 52

// Thermometer icon sprite
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

// Sync icon sprite dimensions
#define syncSprite_W 41
#define syncSprite_H 30

// Sync icon sprite
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

/**
 * @brief Draws a rotated 3D object on the display
 * 
 * @param display Reference to the Adafruit OLED display object
 * @param model The 3D model to be drawn
 * @param objSize The size of the object
 * @param objOffset_x The x offset of the object
 * @param objOffset_y The y offset of the object
 * @param quat The quaternion representing the orientation
 */
void drawRotatedObj(Adafruit_SH1106G &display, Model model, float objSize, float objOffset_x, float objOffset_y, imu::Quaternion quat);

/**
 * @brief Draws a vector graph on the display
 * 
 * @param display Reference to the Adafruit OLED display object
 * @param vector The vector to be drawn
 * @param x_position The x position on the display
 * @param graph_x_size The size of the graph on the x-axis
 * @param xVals Array of x-axis values
 * @param yVals Array of y-axis values
 * @param zVals Array of z-axis values
 */
void drawVectorGraph(Adafruit_SH1106G &display, imu::Vector<3> vector, int x_position, const int graph_x_size, int8_t *xVals, int8_t *yVals, int8_t *zVals);

/**
 * @brief Scales a value logarithmically
 * 
 * @param val The value to be scaled
 * @param sensitivity The sensitivity factor
 * @return The scaled value
 */
int scaleLogarithmically(float val, float sensitivity);

/**
 * @brief Rotates a point using a quaternion
 * 
 * @param x The x coordinate of the point
 * @param y The y coordinate of the point
 * @param z The z coordinate of the point
 * @param quat The quaternion representing the orientation
 */
void rotatePoint(float &x, float &y, float &z, imu::Quaternion quat);

/**
 * @brief Creates a 3D model
 * 
 * @param vertices Array of vertices
 * @param indices Array of indices
 * @param numIndices Number of indices
 * @return The created 3D model
 */
Model createModel(Vertex *vertices, Index *indices, uint8_t numIndices);

/**
 * @brief Task function for rendering
 * 
 * @param pvParameters Pointer to the parameters passed to the task
 */
void renderTask(void *pvParameters);

#endif // RENDERING_H
