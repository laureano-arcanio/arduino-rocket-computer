# Arduino Rocket Flight Computer
An Arduino based hardware and software for model rockets recovery and logging system.

![Model 1](/renders/v2.png)

## How it works ?
Altitude is measured using a Presion BMP280 Sensor. Data is passed through a Kalman filter and then apogee is detected.
Logs are stored in a SD card as CSV format.

## First Rocket Flight
Our first flight deployed parachute before apogee, due to an issue with flight computer executing Emergency mode right after liftoff. Anyways, we recovered the entire rocket without any damage.
[First Flight](https://www.youtube.com/watch?v=o5odCazfV4E)

## Electronics
![Circuito](/schematics/protoboard.png)
![Full Assembly](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Full_assembly_v2.jpeg)
![Full Assembly chute deployed](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Full_chute_deploy_v2.jpeg)
![Electronics Bay open](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Electronics_bay_v2.jpeg)
![Electronics Build](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Electronics_v1.jpeg)
