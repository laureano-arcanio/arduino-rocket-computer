# Arduino Rocket Flight Computer
An Arduino based hardware and software for model rockets recovery and logging system.

![Model 1](/renders/v2.png)

## How it works ?
Altitude is measured using a Presion BMP280 Sensor. Data is passed through a Kalman filter and then apogee is detected.
Logs are stored in a SD card as CSV format.

### First Rocket Flight
**Date:** Saturday, October 15th 2022 18:04pm
**Rocket version:** v46
**Computer firmware:** Commit 24 #9e297cbeb49c01d56837683099e77f7d58b3c18b
**Engine:** 18g KNSU (water re-cristalinization)
**Status:** Partial success, rocket recovered safe. Data logs not stored
**Logs:**
Our first flight deployed parachute before apogee, due to an issue with flight computer executing Emergency mode right after liftoff. Anyways, we recovered the entire rocket without any damage.
[First Flight](https://www.youtube.com/watch?v=o5odCazfV4E)

### Second Rocket Flight
Same as before

### Third Rocket Flight (Cancelled)
**Date:** Sunday, October 16th 2022 18:04pm
**Rocket version:** v46
**Computer firmware:** Commit 24 #9e297cbeb49c01d56837683099e77f7d58b3c18b
**Engine:** 18g KNSU (water re-cristalinization)
**Status:** Cancelled due to low battery and wiring issue.
**Logs:**
Our battery compleately died. We struggle with some bad wiring for a long time beferohand. 

### Fourth Rocket flight
**Date:** Saturday, October 29th 2022 20:15pm
**Rocket version:** v48
**Computer firmware:** Commit 38 ec74b0bc3eabf66b300b1d60ef9739c1b33f2775
**Engine:** 18g KNSU (water re-cristalinization)
**Status:** Failure. Engine failure, Computer failure. 
**Logs:**
We experienced several unknown issues, initially servo lock did not work. This overheated the servo and some wires burnt out. After several attemps, we manage to get computer to ready status, fired the rocket but engine failes at about 5-7 mts hight. Rocket recovered almost 100% safe except for a broken finn. None logs where stored, we figured out 180deg servos don't work well on edges. so we switched from 0 to 25 deg to 15 to 40.

## Fifth Rocket Flight
**Date:** Wednesday, November 9 2022 17hspm
**Weather**: Sunny, Windy (10-15knots)
**Rocket version:** v48
**Computer firmware:** Commit 44 58a310a3b09a8df3bc02ff4712aa3345adc88105
**Engine:** 18g KNSU (Nakka method, caramelization of fine powder grains)
**Status:** Failure. Computer failure. Avionics totally destroyed, computer, parachute and engine recovered
**Logs:**
Our rocket flow perfectly, but parachute did not ejected. After landing, and some further investigation we figured out for some reason the computer switched to Liftoff status before liftoff (probably due to pression change, was a windy day). And due to launchpad conditions switched to landed after 3.5s. So the rocket flown with a computer in Landed status. We'll fix the liftoff condition to account for more meters (>5) and shorten EMERGENCY deploy time to fit this rocket motor. (4s)
[Fifth Flight](https://www.youtube.com/watch?v=9i5LRDVh-5M)


## Electronics
![Circuito](/schematics/protoboard.png)
![Full Assembly](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Full_assembly_v2.jpeg)
![Full Assembly chute deployed](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Full_chute_deploy_v2.jpeg)
![Electronics Bay open](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Electronics_bay_v2.jpeg)
![Electronics Build](https://github.com/laureano-arcanio/arduino-rocket-computer/blob/main/images/Electronics_v1.jpeg)
