/*

  Arduino Rocket Computer

  Authors:
  -------------------
    Laureano Arcanio
    Jeremias Giorgi
    Martiniano Arcanio

  Contact Info
  -------------------
    laureano.arcanio@gmail.com

    
    Based off the work of Danilo Nascimento / ndanilo8@hotmail.com
*/


#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <MPU6050.h>
#include <Servo.h>

#define DEBUG //uncomment to start debug mode

// Modules
Servo servo;
MPU6050 mpu;
Adafruit_BMP280 bmp;
int16_t ax, ay, az;

const int chipSelect = 10;

// Status monitur interval
unsigned long interval = 1000; // timer for buzzer
long previousMillis = 0;
unsigned long currentMillis = 0;

//ground level altitude
long initialAltitude;
// Ground level Presssure 
float P0;

//current altitude
long currAltitude;
float rawAltitude = 0;

//Apogee altitude
long liftoffAltitude = 2;
long lastAltitude = 0;

boolean liftoff = false;
boolean apogeeHasFired = false;
boolean landed = false;

boolean err = false; //Error check

// consecutive measures < apogee to run before apogee confirmation
unsigned long measures = 3;

// Pin Out
const int redLed = 9;
const int greenLed = 8;
const int blueLed = 7;
const int pinApogee = A0;
const int buzzer = 6;

//*********Kalman filter Variables*****************
float f_1 = 1.00000;
float kalman_x;
float kalman_x_last;
float kalman_p;
float kalman_p_last;
float kalman_k;
float kalman_q;
float kalman_r;
float kalman_x_temp;
float kalman_p_temp;

void setup()
{
  KalmanInit();

  #ifdef DEBUG
    Serial.begin(115200);
  #endif
    Wire.begin();

  SDcardInit(); //SD card: see if the card is present and can be initialized:
  bmpInit(); //Pressure Sensor Init & Check
  mpuInit(); //Accel Sensor Init & Check

  //Initialise the Pyro & buzzer pins)
  pinMode(pinApogee, OUTPUT);
  pinMode(buzzer, OUTPUT);

  //Make sure that the output are turned off
  digitalWrite(pinApogee, LOW);
  digitalWrite(buzzer, LOW);

  //LED pins
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  servo.attach(pinApogee, 1000, 2000);

  //number of measures to do to detect Liftoff & Apogee
  P0 =  bmp.readPressure() / 100;

  // TODO debug this, not sure its needed
  // Do some dummy altitude reading
  // to initialise the Kalman filter
  for (int i = 0; i < 50; i++) {
    KalmanCalc(bmp.readAltitude(P0));
  }

  //Read the lauch site altitude
  long sum = 0;
  long curr = 0;
  for (int i = 0; i < 10; i++) {
    curr = KalmanCalc(bmp.readAltitude(P0));
    sum += curr;
    delay(50);
  }

  initialAltitude = (sum / 10.0);

  if (err != true)
  {
    init_Led();
  }
}

void loop()
{
  currentMillis = millis();
  statusMonitor();
  if (err == true)
  {
    return;
  }

  rawAltitude = bmp.readAltitude(P0);
  currAltitude = (KalmanCalc(rawAltitude) - initialAltitude);

  // Detect Liftoff
  if ((currAltitude > liftoffAltitude) && liftoff == false)
  {
    liftoff = true;
  }

  //detect Apogee
  if (liftoff == true)
  {
    if (currAltitude >= lastAltitude )
    {
      lastAltitude = currAltitude;
    }
    else
    {
      if (measures == 0 and !apogeeHasFired)
      {
        apogeeHasFired = true;
      }
      else
      {
        measures -= 1;
      }
    }
  }

  // Deploy Parachute / Rescue secuence
  if (apogeeHasFired == true)
  {
    // Eject nose cone with servo
    servo.write(360);
  }

  // Detect Landing
  if (apogeeHasFired == true && liftoff == true && currAltitude <= liftoffAltitude)
  {
    liftoff = false;
    landed = true;
  }

  dataLogger();
}