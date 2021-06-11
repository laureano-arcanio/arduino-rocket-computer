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

*/


#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <MPU6050.h>
//#include <Servo.h>
#include <SimpleTimer.h>

// Modules
SimpleTimer timer;
//Servo servo;
MPU6050 mpu;
Adafruit_BMP280 bmp;
int16_t ax, ay, az;

const int chipSelect = 10;

boolean allOn = true;

// Ground level Presssure 
float P0;
float currAltitude;
float rawAltitude = 0;

float initialAltitude = 0;
float lastAltitude = 0;
float prevAltitude = 0;

// Status variable
const unsigned int STATUS_READY = 20;
const unsigned int STATUS_LIFTOFF = 40;
const unsigned int STATUS_APOGEE = 60;
const unsigned int STATUS_LANDED = 80;

unsigned int status = 20;
  // 1x Errors
  // 10 = SD Error
  // 11 = BMP Error
  // 12 = MPU

  // 20 = Ready
  // 40 = Lisftoff 
  // 60 = Apogee
  // 80 = Landed 

boolean err = false; //Error check

// consecutive measures < apogee to run before apogee confirmation
int measures = 15;

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
  Serial.begin(9600);
  Wire.begin();
  
  timer.setInterval(500, statusMonitor);

  KalmanInit();
 
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

  // servo.attach(pinApogee, 1000, 2000);

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

  if (status >= 20)
  {
    status = STATUS_READY;
    init_Led();
  }
}

void loop()
{ 
  // Serial.println(F("Status: "));
  // Serial.print(status);
  timer.run();

  if (status < STATUS_READY)
  {
    return;
  }

  rawAltitude = bmp.readAltitude(P0);
  currAltitude = KalmanCalc(rawAltitude) - initialAltitude;

  // Detect Liftoff
  if ((currAltitude > initialAltitude + 1) && status == STATUS_READY)
  {
    //Serial.println("Lift Off");
    status = STATUS_LIFTOFF;
  }

  // //detect Apogee
  if (status == STATUS_LIFTOFF)
  {
    if (currAltitude >= lastAltitude)
    {
      lastAltitude = currAltitude;
      measures = 15;
    }
    else
    {
      if (measures == 0)
      {
        status = STATUS_APOGEE;
        //Serial.println("Apogee");
      }
      else
      {
        if (prevAltitude != currAltitude)
        {
          measures -= 1;
        }
      }
    }
  }


  // Deploy Parachute / Rescue secuence
  if (status == STATUS_APOGEE)
  {
    // Eject nose cone with servo
    // servo.write(360);
  }

  // Detect Landing
  if (status == STATUS_APOGEE)
  {
    if (abs(currAltitude - initialAltitude) < 2)
    {
      status = STATUS_LANDED;
      //Serial.println("landed");
    }
  }

  if (status >= STATUS_LIFTOFF and status < STATUS_LANDED)
  {
    dataLogger();
  }
}
