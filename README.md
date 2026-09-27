## VTal Fitness Tracker
VTal is a wearable fitness tracker that utilises MAX30102, BMP390, and LSM6DSOX sensors to monitor a user's physical activity and vital signs. Powered by an STM32 (NUCLEO-L432KC) microcontroller, the device streams real-time biometric and kinematic data via an HC-05 Bluetooth module to a custom-made app, which utilises Qt6 for visualisation.

## Key Features
* **Biometric Monitoring:** Real-time pulse oximetry and heart rate tracking.
* **Environmental and Motion Tracking:** Integrated barometric pressure sensing and 6-axis IMU.
* **Data Analysis:** Calorie and distance counter based on acquired biometric data.
* **Wireless Telemetry:** Serial over Bluetooth data transmission to the host application.
* **Custom Mobile App:** Data visualisation on the user's mobile phone.

## Hardware Requirements
* NUCLEO-L432KC Development Board
* MAX30102 SP02 and Pulse Sensor
* BMP390 Pressure and Temperature Sensor
* LSM6DSOX IMU
* HC-05 Bluetooth Module
* 5V Power Source 
* Custom 3D Printed Enclosure

## Software Prerequisites
* STM32CubeIDE
* STM32CubeMX
* Qt6

## Pinout
