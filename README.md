# Arduino Rocket Flight Computer
An Arduino based hardware and software for model rockets recovery and logging system.

![Model 1](/renders/v2.png)

## How it works ?
Altitude is measured using a Presion BMP180 Sensor. Data is passed through a Kalman filter and then apogee is detected after 3 consecutive altitude drops.
Logs are stored in a SD card as CSV format, with 1kb redced EEPROM storage as backup (204 rows of 5 bytes).

Parachute deployment is done using a 180º Servo motor

Check out the circuit [here](/schematics/protoboard.png) 

### Full Assembly
![Full Assembly](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/full_assembly_2.jpg)

### STL Files
![Stl files](https://github.com/laureano-arcanio/arduino-rocket-computer/tree/main/STLs)
All body parts are printed with 1 wall, and 10% infill except for the payload area that needs supports for the servo mount. 


### The Team
![The Team](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/rocket_team_2.jpg)

## Check all our FLights and Log [here](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/LOGS.md)
