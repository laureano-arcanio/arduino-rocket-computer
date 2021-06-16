# Arduino Rocket Flight Computer
An Arduino based hardware and software for model rockets recovery and logging system.

## How it works ?
Altitude is measured using a Presion BMP280 Sensor. Data is passed through a Kalman filter and then apogee is detected.
Logs are stored in a SD card as CSV format.

