# Wireless Motion Sensor

A portable all-in-one motion sensor device with Bluetooth connectivity and SD card data logging capabilities, developed as a Bachelor's thesis project at UAS Technikum Wien.

## Overview

This project presents a low-cost, user-friendly wireless motion tracking device designed for measuring acceleration, rotation, and orientation data at 100 Hz using a Bosch BNO055 IMU sensor. The device features wireless communication between two units, local data storage, and an intuitive OLED display interface.

![Fully Assembled Device](images/Fully%20assembled%20device%20with%20case.png)

## Key Features

- **High-Speed Motion Capture**: Measures acceleration, rotation, and orientation at 100 Hz
- **Wireless Communication**: Bluetooth Classic connectivity for real-time data sharing between two devices
- **Data Logging**: SD card storage in CSV format for easy data access
- **Clock Synchronization**: Devices can be synchronized by tapping them together (high-g interrupt method)
- **User-Friendly Interface**: 1.3" OLED display with push-button controls
- **Portable Design**: Battery-powered with 3D-printed snap-fit case
- **Multiple Operation Modes**:
  - Fusion Mode (orientation visualization)
  - Temperature Mode
  - Spirit Level Mode
  - Raw Readings Mode

## Hardware Components

### Main Components
- **Microcontroller**: ESP32-WROOM-32
- **IMU Sensor**: Bosch BNO055 (9-DOF with sensor fusion)
- **Display**: SH1106 1.3" OLED (128x64 pixels)
- **Storage**: MicroSD card slot
- **Power**: Li-Ion/Li-Polymer battery with USB-C charging
- **Connectivity**: Bluetooth Classic

### PCB Design

![PCB without OLED](images/PCB%20without%20OLED.png)
![PCB with OLED](images/PCB%20with%20OLED.png)

The custom 4-layer PCB features:
- Compact design (96.6mm x 44.4mm x 19.9mm)
- Hand-assembled components
- Compatible with various 0.96" or 1.3" OLED displays
- M5 mounting holes for case integration

### Circuit Schematic

![Circuit Schematic Page 1](images/KiCad%20circuit%20schematic%20page%201.png)
![Circuit Schematic Page 2](images/KiCad%20circuit%20schematic%20page%202.png)

## Case Design

The device is housed in a 3D-printed snap-fit case designed in Autodesk Fusion:

![Case Render](images/Autodesk%20Fusion%20case%20render.png)

Features:
- Snap-fit assembly (no screws required)
- Integrated battery compartment
- Button and switch caps
- Multi-material printing support for labeling

## Software Architecture

The firmware uses FreeRTOS with seven concurrent tasks:
- **Button Task**: Handles user input
- **Sensor Task**: Reads IMU data and manages clock synchronization
- **Render Task**: Controls OLED display output
- **Bluetooth TX/RX Tasks**: Manages wireless communication
- **SD Card Task**: Handles file operations
- **CSV Task**: Prepares data for logging with write buffer optimization

### Data Recording

- CSV format for easy accessibility
- Buffered writing to reduce SD card wear
- Simultaneous local and remote data logging
- Timestamp synchronization between devices

## Operation Modes

1. **Fusion Mode**: 3D cube visualization of orientation with linear acceleration graph
2. **Temperature Mode**: Display local and remote temperature readings
3. **Spirit Level Mode**: Digital level indicator with tilt angle
4. **Raw Readings Mode**: Real-time graphs of accelerometer, gyroscope, and magnetometer data
5. **Clock Sync Mode**: Synchronize device clocks by tapping them together

## Getting Started

### Hardware Requirements
- Wireless Motion Sensor device
- MicroSD card (formatted as FAT32)
- Li-Ion or Li-Polymer battery (JST 2.54mm connector)
- USB-C cable for charging and programming

### Basic Usage

1. **Power On**: Slide the ON/OFF switch to ON position
2. **Insert SD Card**: Insert a FAT32-formatted microSD card (optional, for data logging)
3. **Connect Devices**: Press the BT button on one device to establish connection
4. **Select Mode**: Press MODE button to cycle through operation modes
5. **Record Data**: Press REC button to start/stop data logging
6. **Sync Clocks**: Press SYNC button (on master device) and tap devices together

### Controls

- **MODE**: Switch between operation modes
- **BT**: Initiate/disconnect Bluetooth connection
- **REC**: Start/stop CSV data logging
- **SYNC**: Begin clock synchronization process (master device only)

## Performance

- **Synchronization Accuracy**: Typically within 5ms
- **Bluetooth Latency**: ~15ms average
- **Data Rate**: 100 Hz for all sensor readings
- **Packet Loss**: ~24% during simultaneous Bluetooth and SD card operations

## Applications

- Motion capture and tracking
- Sports performance analysis
- Gait analysis and rehabilitation
- Robotics and drone motion tracking
- Joint movement studies
- Vibration analysis

## Development

### Build Environment
- Platform: PlatformIO with Arduino framework
- Language: C/C++
- RTOS: FreeRTOS

### PCB Design
- Tool: KiCad
- Manufacturer: Aisler
- Layers: 4 (Signal, Ground, Power, Signal)

### Mechanical Design
- Tool: Autodesk Fusion
- Manufacturing: 3D printing (FDM)

## Project Structure

```
WirelessMotionSensor/
├── src/                    # Source code
├── include/                # Header files
├── KiCad/                  # PCB design files
├── CAD/                    # 3D case design files
├── images/                 # Documentation images
└── platformio.ini          # PlatformIO configuration
```

## Future Improvements

- Enhanced communication protocol (ESP-NOW) for multiple device support
- Optimized code for better performance
- Additional display interfaces
- Live data logging over USB
- Support for more than two connected devices

## License

This project was developed as a Bachelor's thesis at UAS Technikum Wien.

## Author

**Alper Ramadan**
Bachelor of Science in Engineering - Electronics
UAS Technikum Wien, 2024

Supervisor: Stefan Paschek, MSc

## Acknowledgments

Special thanks to:
- UAS Technikum Wien for providing education and resources
- Jakob Frenzel for PCB design guidance
- Adafruit and AZ-Delivery for open-source hardware designs

## References

Based on the Bachelor's thesis: "Development of a Portable Motion Sensor Device with Bluetooth and SD Card Capabilities"
