#ifndef BLUETOOTH_H
#define BLUETOOTH_H

// Lower connection timeout of the library
#include "BluetoothSerial.h"
#include "commons.h"

#define PACKET_SIZE 88  // Bluetooth packet size (must be higher than the biggest struct that can be sent and <=512)
#define BT_LED_PIN 16  // Pin used to indicate BT communication without toggling too fast

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

const String bluetoothName = "WirelessMotionSensor";

/**
 * @brief Sends data over Bluetooth
 * 
 * @param SerialBT reference to the BluetoothSerial object
 * @param data pointer to the data to be sent
 * @param dataSize size of the data to be sent
 */
void sendBT(BluetoothSerial& SerialBT, const void* data, size_t dataSize);

/**
 * @brief Receives data over Bluetooth
 * 
 * @param SerialBT reference to the BluetoothSerial object
 * @param data Pointer to the buffer where received data will be stored
 * @param dataSize Size of the buffer
 */
void receiveBT(BluetoothSerial& SerialBT, void* data, size_t dataSize);

/**
 * @brief Sends a structured data packet over Bluetooth
 * 
 * @param SerialBT reference to the BluetoothSerial object
 * @param id Character identifier for the data packet
 * @param data Pointer to the structured data to be sent
 * @param dataSize Size of the structured data to be sent
 */
void sendStruct(BluetoothSerial &SerialBT, const char id, const void *data, size_t dataSize);

/**
 * @brief Receives a structured data packet over Bluetooth
 * 
 * @param SerialBT reference to the BluetoothSerial object
 */
void receiveStruct(BluetoothSerial &SerialBT);

/**
 * @brief Task function for receiving data over Bluetooth
 * 
 * @param pvParameters Pointer to the parameters passed to the task
 */
void bluetoothRXTask(void* pvParameters);

/**
 * @brief Task function for sending data over Bluetooth
 * 
 * @param pvParameters Pointer to the parameters passed to the task
 */
void bluetoothTXTask(void* pvParameters);

/**
 * @brief Unpairs the Bluetooth device
 * 
 * @param SerialBT reference to the BluetoothSerial object
 */
void unpairBT(BluetoothSerial &SerialBT);

// 
extern TaskHandle_t bluetoothRXTaskHandle;
extern TaskHandle_t bluetoothTXTaskHandle;
extern BluetoothSerial SerialBT;

#endif // BLUETOOTH_H
