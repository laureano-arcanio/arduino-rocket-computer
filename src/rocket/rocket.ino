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

    Based on work from
    Boris du Reau / https://github.com/bdureau
    Danilo Nascimento / ndanilo8@hotmail.com

*/


#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <MPU6050.h>
#include <Servo.h>
#include <SimpleTimer.h>

// Modules
SimpleTimer timer;
Servo servo;
MPU6050 mpu;
Adafruit_BMP280 bmp;
int16_t ax, ay, az;
File root;

boolean allOn = true;
String filename = "LOGS_1.txt";

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

//
const unsigned int SECURITY_DEPLOYMENT_TIME = 10000;

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
unsigned int measures = 15;

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

String getFileName(File dir, int numTabs) {
  unsigned int fileCount = 1;
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      break;
    }
    fileCount++;
    entry.close();
  }
  return "LOGS_" + String(fileCount) + ".txt";
}  

void setup()
{
  Wire.begin();
 
  // Modules Init and checks
  if (!SD.begin(10))
  {
    //Serial.println("Card failed, or not present");
    status = 10;
  }
  else {
    root = SD.open("/");
    filename = getFileName(root, 0);
  }

  if (!bmp.begin())
  {
    //Serial.println("BMP sensor failed!");
    status = 11;
  }

  mpu.initialize(); //Accel/gyro Sensor Initialisation
  if (!mpu.testConnection())
  {
    //Serial.println("MPU Failed!");
    status = 12;
  }

  //Initialise the Pyro & buzzer pins)
  pinMode(pinApogee, OUTPUT);
  //pinMode(buzzer, OUTPUT);

  //Make sure that the output are turned off
  //digitalWrite(buzzer, LOW);

  //LED pins
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  // Status timers
  timer.setInterval(500, statusMonitor);

  KalmanInit();
  P0 =  bmp.readPressure() / 100;

  // initialise the Kalman filter
  for (int i = 0; i < 50; i++) {
    KalmanCalc(bmp.readAltitude(P0));
  }

  //Read the lauch site altitude
  float sum = 0;
  float curr = 0;
  for (int i = 0; i < 10; i++) {
    curr = KalmanCalc(bmp.readAltitude(P0));
    sum += curr;
    delay(50);
  }

  initialAltitude = (sum / 10.0);

  if (status == STATUS_READY)
  {
    status = STATUS_READY;
    init_Led();
    // Initialize servo after all delays
    // Servos won't play well when using delay
    servo.attach(pinApogee);
    servo.write(25);
  }
}

void loop()
{ 
  timer.run();

  if (millis() > 10000)
//  {
//   servo.write(0);
//  }
//  return;

  if (status < STATUS_READY)
  {
    return;
  }

  rawAltitude = bmp.readAltitude(P0);
  currAltitude = KalmanCalc(rawAltitude) - initialAltitude;

  // Detect Liftoff
  if ((currAltitude > initialAltitude + 1) && status == STATUS_READY)
  {
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
    deployParachute();
  }

  // Make sure to deploy parachute in case all fails
  if (status == STATUS_LIFTOFF and millis() >= SECURITY_DEPLOYMENT_TIME)
  {
    deployParachute();
  }

  // Detect Landing
  if (status == STATUS_APOGEE)
  {
    if (abs(currAltitude - initialAltitude) < 2)
    {
      status = STATUS_LANDED;
    }
  }

  if (status >= STATUS_LIFTOFF and status < STATUS_LANDED)
  {
    dataLogger();
  }
}


void deployParachute()
{
  // 25 deg close
  // 0 deg Open
  servo.write(0);
}
