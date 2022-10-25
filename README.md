# Arduino Rocket Flight Computer
An Arduino based hardware and software for model rockets recovery and logging system.

![Model 1](/renders/v2.png)

## How it works ?
Altitude is measured using a Presion BMP280 Sensor. Data is passed through a Kalman filter and then apogee is detected.
Logs are stored in a SD card as CSV format.


## Electronics
![Circuito](/schematics/protoboard.png)
![Electronics Build v1](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Electronics_v1.jpeg)
