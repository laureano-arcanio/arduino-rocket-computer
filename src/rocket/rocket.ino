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

// define SERIAL_DEBUG true

Servo servo;
MPU6050 mpu;
Adafruit_BMP085 bmp;
int ax, ay, az, gx, gy, gz;

File root;

boolean useMPUData = true;
boolean allOn = true;
String filename;
float P0;
int currAltitude;
float rawAltitude = 0;
float rawTemp = 0;
boolean hasHeaders = false;

float initialAltitude = 0;
float lastAltitude = 0;

unsigned long millisAtLiftoff = 0;
unsigned long millisFromLastBlink = 0;
boolean apogeeHasFired = false;

// Status variable
const unsigned int STATUS_READY = 20;
const unsigned int STATUS_LIFTOFF = 40;
const unsigned int STATUS_APOGEE = 60;
const unsigned int STATUS_EMERGENCY_DEPLOY = 70;
const unsigned int STATUS_LANDED = 80;
unsigned int status = STATUS_READY;
// 1x Errors
// 10 = SD Error
// 11 = BMP Error
// 12 = MPU

// 20 = Ready
// 40 = Lisftoff
// 60 = Apogee
// 80 = Landed

// Security deploy time
const unsigned int SECURITY_DEPLOYMENT_TIME = 15000;

// consecutive measures < apogee to run before apogee confirmation
unsigned int measures = 2;

// Pin Out
const unsigned int redLed = 9;
const unsigned int greenLed = 8;
const unsigned int blueLed = 7;
const unsigned int intLed = 13;
const int unsigned pinApogee = A0;
const int unsigned buzzer = 6;

void statusMonitor()
{
  if (allOn == true)
  {
    allOn = false;
    if (status == STATUS_READY)
    {
      digitalWrite(greenLed, HIGH);
    }
    if (status == STATUS_LIFTOFF)
    {
      digitalWrite(blueLed, HIGH);
    }
    if (status == STATUS_APOGEE)
    {
      digitalWrite(blueLed, HIGH);
      digitalWrite(greenLed, HIGH);
    }
    if (status == STATUS_LANDED)
    {
      digitalWrite(redLed, HIGH);
      digitalWrite(blueLed, HIGH);
      digitalWrite(greenLed, HIGH);
      tone(buzzer, 880);
    }
    if (status == STATUS_EMERGENCY_DEPLOY)
    {
      digitalWrite(redLed, HIGH);
    }

    if (status < 20)
    {
      if (status == 10)
      {
        digitalWrite(redLed, HIGH);
        digitalWrite(greenLed, HIGH);
      }
      if (status == 11)
      {
        digitalWrite(redLed, HIGH);
        digitalWrite(blueLed, HIGH);
      }
      // if (status == 12 ) {
      //   digitalWrite(LED_BUILTIN, HIGH);
      // }
      tone(buzzer, 880);
    }
  }
  else
  {
    allOn = true;
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(intLed, LOW);
    noTone(buzzer);
  }
}

String getFileName(File dir, int numTabs)
{
  unsigned int fileCount = 1;
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      break;
    }
    fileCount++;
    entry.close();
  }
  return "LOGS_" + String(fileCount) + ".txt";
}

void dataLogger()
{
  if (useMPUData == true)
  {
    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);
  }

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile)
  {
    if (hasHeaders == false)
    {
      // CSV format:
      // Millis from init, Status, Initial Altitude, Current Altitude (by kalman), Raw Altitude measurement, Acc X, Acc Y, Acc Z, Gyro X, Gyro Y, Giro Z
      dataFile.println("Ms\tCode\tA0\tCA\tMA\tT\tAX\tAY\tAZ\tGX\tGY\tGZ");
      hasHeaders = true;
    }
    dataFile.print(millis());
    dataFile.print("\t");
    dataFile.print(status);
    dataFile.print("\t");
    dataFile.print(initialAltitude);
    dataFile.print("\t");
    dataFile.print(currAltitude);
    dataFile.print("\t");
    dataFile.print(rawAltitude);
    dataFile.print("\t");
    dataFile.print(rawTemp);
    dataFile.print("\t");
    dataFile.print(ax / 2048);
    dataFile.print("\t");
    dataFile.print(ay / 2048);
    dataFile.print("\t");
    dataFile.print(az / 2048);
    dataFile.print("\t");
    dataFile.print(gx);
    dataFile.print("\t");
    dataFile.print(gy);
    dataFile.print("\t");
    dataFile.print(gz);
    dataFile.print("\t");
    dataFile.print("\n");
    dataFile.close();
  }
}

void lockParachuteSequence()
{
  // All led high servo open for parachute insertion
  digitalWrite(redLed, HIGH);
  digitalWrite(blueLed, HIGH);
  digitalWrite(greenLed, HIGH);
  servo.attach(pinApogee);
  servo.write(15);
  delay(8000);
  servo.write(40);
  // Leds off after servo lock
  digitalWrite(redLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(greenLed, LOW);
}

void deployParachute()
{
  // 40 deg close
  // 15 deg Open
  if (apogeeHasFired == false)
  {
    servo.write(15);
    apogeeHasFired = true;
  }
}

void setup()
{
#if SERIAL_DEBUG
  Serial.begin(9600);
#endif

  Wire.begin();

  // LED pins
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(intLed, OUTPUT);
  pinMode(pinApogee, OUTPUT);

  // Led / Buzzer test sequence
  digitalWrite(intLed, HIGH);
  delay(300);
  digitalWrite(redLed, HIGH);
  delay(300);
  digitalWrite(greenLed, HIGH);
  delay(300);
  digitalWrite(blueLed, HIGH);
  delay(300);
  digitalWrite(redLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(intLed, LOW);

  // Buzzer test
  tone(buzzer, 440);
  delay(500);
  noTone(buzzer);

  // Modules Init and checks
  if (!SD.begin(10))
  {
#if SERIAL_DEBUG
    Serial.println("Card failed, or not present");
#endif
    status = 10;
  }
  else
  {
    root = SD.open("/");
    filename = getFileName(root, 0);
  }

  if (!bmp.begin(10))
  {
#if SERIAL_DEBUG
    Serial.println("BMP sensor failed!");
#endif
    status = 11;
  }

  // MPU Sensor Initialisation
  mpu.initialize();
  if (!mpu.testConnection())
  {
#if SERIAL_DEBUG
    Serial.println("MPU Failed!");
#endif
    // If it fails, it won't stop working
    // logs will not fetch MPU data
    // status = 12;
    useMPUData = false;
  }

  // Averagely measured P0
  float pressoureAccum = 0;
  for (int i = 0; i < 10; i++)
  {
    delay(10);
    pressoureAccum += bmp.readPressure();
  }

  P0 = pressoureAccum / 10;

  // Averagely measured Altitude
  float altitudeAccum = 0;
  for (int i = 0; i < 10; i++)
  {
    altitudeAccum += KalmanCalc(bmp.readAltitude(P0));
    delay(10);
  }
  initialAltitude = int(altitudeAccum / 10.0);

  // initialise the Kalman filter
  KalmanInit();
  for (int i = 0; i < 50; i++)
  {
    delay(10);
    KalmanCalc(bmp.readAltitude(P0));
  }

#if SERIAL_DEBUG
  Serial.println("P0 value: " + String(P0));
  Serial.println("Iniotial Altitude value: " + String(initialAltitude));
  Serial.println("Status: " + String(status));
#endif

  if (status == STATUS_READY)
  {
    lockParachuteSequence();
  }
  millisFromLastBlink = millis();
}

void loop()
{
  if (millis() - millisFromLastBlink >= 500)
  {
    statusMonitor();
    millisFromLastBlink = millis();
  }

  if (status < STATUS_READY || status == STATUS_LANDED)
  {
    return;
  }
  rawTemp = bmp.readTemperature();
  rawAltitude = bmp.readAltitude(P0);
  currAltitude = KalmanCalc(rawAltitude) - initialAltitude;

#if SERIAL_DEBUG
  Serial.println("Raw Altitude " + String(rawAltitude));
  Serial.println("Kalman Altitude " + String(rawTemp));
  Serial.println("Previous Altitude: " + String(lastAltitude));
  Serial.println("Raw Temp: " + String(rawTemp));
  Serial.println("Status: " + String(status));
#endif

  // Liftoff detection
  if ((currAltitude > initialAltitude + 1) && status == STATUS_READY)
  {
    status = STATUS_LIFTOFF;
    millisAtLiftoff = millis();
  }

  // Apogee detection
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

  // Emergency parachute deployment
  if (status == STATUS_LIFTOFF and millisAtLiftoff > 0 and (millis() - millisAtLiftoff) >= SECURITY_DEPLOYMENT_TIME)
  {
    deployParachute();
    status = STATUS_EMERGENCY_DEPLOY;
  }

  // if (status >= STATUS_LIFTOFF and status < STATUS_LANDED)
  // {
  dataLogger();
  // }
}
