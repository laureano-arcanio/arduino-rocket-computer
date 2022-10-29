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

*/

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h>
#include <MPU6050.h>
#include <Servo.h>

Servo servo;
MPU6050 mpu;
Adafruit_BMP085 bmp;
int ax, ay, az, gx, gy, gz;

File root;

boolean allOn = true;
String filename;

// Ground level Presssure 
float P0;
int currAltitude;
float rawAltitude = 0;

float initialAltitude = 0;
float lastAltitude = 0;

unsigned long millisAtLiftoff = 0;
unsigned long millisFromLastBlink = 0;
boolean  apogeeHasFired = false;

// Status variable
const unsigned int STATUS_READY = 20;
const unsigned int STATUS_LIFTOFF = 40;
const unsigned int STATUS_APOGEE = 60;
const unsigned int STATUS_EMERGENCY_DEPLOY = 70;
const unsigned int STATUS_LANDED = 80;


//
const unsigned int SECURITY_DEPLOYMENT_TIME = 15000;

unsigned int status = 20;
  // 1x Errors
  // 10 = SD Error
  // 11 = BMP Error
  // 12 = MPU

  // 20 = Ready
  // 40 = Lisftoff 
  // 60 = Apogee
  // 80 = Landed 

// consecutive measures < apogee to run before apogee confirmation
unsigned int measures = 2;

// Pin Out
const unsigned int redLed = 9;
const unsigned int greenLed = 8;
const unsigned int blueLed = 7;
const int unsigned pinApogee = A0;
const int unsigned buzzer = 6;

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
  //Serial.begin(9600);
  
  //LED pins
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  // Led / Buzzer test sequence
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(redLed, HIGH);
  delay(300);
  digitalWrite(greenLed, HIGH);
  delay(500);
  digitalWrite(blueLed, HIGH);
  delay(300);
  digitalWrite(redLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(greenLed, LOW);

  // Buzzer test
  tone(buzzer, 440);
  delay(500);
  noTone(buzzer);

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

  //Initialise the Pyro & buzzer pins)
  pinMode(pinApogee, OUTPUT);
  KalmanInit();
  float pressoureAccum = 0;
  for (int i = 0; i < 10; i++) {
    delay(100);
    pressoureAccum += bmp.readPressure();
  }

  P0 = pressoureAccum / 10;

  // initialise the Kalman filter
  for (int i = 0; i < 50; i++) {
    delay(10);
    KalmanCalc(bmp.readAltitude(P0));
  }

  //Read the lauch site altitude
  float sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += KalmanCalc(bmp.readAltitude(P0));;
    delay(50);
  }

  initialAltitude = int(sum / 10.0);
  //Serial.println("Initial P");
  //Serial.println(P0);
  //Serial.println("Initial Altitude");
  //Serial.println(initialAltitude);

  if (status == STATUS_READY)
  {
    status = STATUS_READY;
    // Initialize servo after all delays
    // Servos won't play well when using delay
    servo.attach(pinApogee);
    // All led high servo open for parachute insertion
    digitalWrite(redLed, HIGH);
    digitalWrite(blueLed, HIGH);
    digitalWrite(greenLed, HIGH);
    servo.write(0);
    delay(5000);
    servo.write(25);
    // Leds off after servo lock
    digitalWrite(redLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, LOW);
  }
  millisFromLastBlink = millis();
}

void loop()
{ 
  if (millis() - millisFromLastBlink >= 500) {
    statusMonitor();
    millisFromLastBlink = millis();
  } 
  
  if (status < STATUS_READY || status == STATUS_LANDED)
  {
    return;
  }

  rawAltitude = bmp.readAltitude(P0);
  currAltitude = KalmanCalc(rawAltitude) - initialAltitude;

  // Detect Liftoff
  if ((currAltitude > initialAltitude + 1) && status == STATUS_READY)
  {
    status = STATUS_LIFTOFF;
    millisAtLiftoff = millis();
  }

  // //detect Apogee
  if (status == STATUS_LIFTOFF)
  {
    if (currAltitude > lastAltitude)
    {
      measures = 2;
    }
    else
    {
      if (measures == 0)
      {
        status = STATUS_APOGEE;
      }
      else
      {
        if (lastAltitude != currAltitude)
        {
          measures -= 1;
        }
      }
    }
    lastAltitude = currAltitude;
  }

  // Deploy Parachute / Rescue secuence
  if (status == STATUS_APOGEE)
  {
    // Eject nose cone with servo
    deployParachute();
  }
  
  // Detect Landing
  if (status == STATUS_APOGEE || status == STATUS_EMERGENCY_DEPLOY)
  {
    if (abs(currAltitude - initialAltitude) < 2)
    {
      status = STATUS_LANDED;
    }
  }

  // Make sure to deploy parachute in case all fails
  if (status == STATUS_LIFTOFF and millisAtLiftoff > 0 and (millis() - millisAtLiftoff) >= SECURITY_DEPLOYMENT_TIME)
  {
    deployParachute();
    status = STATUS_EMERGENCY_DEPLOY;
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
  if (apogeeHasFired == false) {
    servo.write(0);
    apogeeHasFired = true;
  }
  
}
