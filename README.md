# Arduino Rocket Flight Computer
An Arduino based hardware and software for model rockets recovery and logging system.

![Model 1](/renders/v2.png)

## How it works ?
Altitude is measured using a Presion BMP280 Sensor. Data is passed through a Kalman filter and then apogee is detected.
Logs are stored in a SD card as CSV format.

## First Rocket Flight
Our first flight deployed parachute before apogee, due to an issue with flight computer executing Emergency mode right after liftoff. Anyways, we recovered the entire rocket without any damage.
[First Flight](https://www.youtube.com/watch?v=o5odCazfV4E)

## Third Rocet Flight (commit 45)
Our rocket flow perfectly, but parachute did not ejected. After landing, and some further investigation we figured out for some reason the computer switched to Liftoff status before liftoff (probably due to pression change, was a windy day). And due to launchpad conditions switched to landed after 3.5s. So the rocket flown with a computer in Landed status. We'll fix the liftoff condition to account for more meters (>5) and shorten EMERGENCY deploy time to fit this rocket motor. (4s)
[Third Flight](https://www.youtube.com/watch?v=9i5LRDVh-5M)

## Electronics
![Circuito](/schematics/protoboard.png)
![Full Assembly](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Full_assembly_v2.jpeg)
![Full Assembly chute deployed](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Full_chute_deploy_v2.jpeg)
![Electronics Bay open](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Electronics_bay_v2.jpeg)
![Electronics Build](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Electronics_v1.jpeg)
