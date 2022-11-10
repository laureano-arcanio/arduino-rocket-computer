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
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP085.h>
#include <MPU6050.h>
#include <Servo.h>

#define SERIAL_DEBUG true

Servo servo;
MPU6050 mpu;
Adafruit_BMP085 bmp;
int ax, ay, az, gx, gy, gz;

File root;

boolean useMPUData = true;
boolean allOn = true;
String filename;
float P0;
float initialAltitude = 0;
int previousLoopAltitude = 0;
int currentLoopAltitude = 0;
float rawAltitude = 0;
float rawTemp = 0;
boolean hasHeaders = false;


unsigned long millisAtLiftoff = 0;
unsigned long millisFromLastBlink = 0;
unsigned long millisAtCurrentLoop = 0;
unsigned long millisAtLoopStart = 0;
bool apogeeHasFired = false;

// Time interval to check for landed condition
const unsigned short LANDED_CHECK_INTERVAL = 1000;
const unsigned short LANDED_THRESHOLD = 2;
unsigned short millisFromLastLandedCheck = 0;

unsigned short landedCheckCounts = LANDED_THRESHOLD;


// Eeprom variables
unsigned short eepromAvalableBytes = 1024;

// Status variable
const unsigned short STATUS_READY = 20;
const unsigned short STATUS_LIFTOFF = 40;
const unsigned short STATUS_APOGEE = 60;
const unsigned short STATUS_EMERGENCY_DEPLOY = 70;
const unsigned short STATUS_LANDED = 80;
unsigned short status = STATUS_READY;
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

const unsigned short APOGEE_THRESHOLD = 2;
unsigned int consecutiveAltitudeDrop = APOGEE_THRESHOLD;

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

void sdDataLogger()
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
    dataFile.print(millisAtCurrentLoop);
    dataFile.print("\t");
    dataFile.print(status);
    dataFile.print("\t");
    dataFile.print(initialAltitude);
    dataFile.print("\t");
    dataFile.print(currentLoopAltitude);
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

void writeIntIntoEEPROM(int address, int number)
{ 
  EEPROM.update(address, number >> 8);
  EEPROM.update(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}


void eepromDataLogger() {
  // Log flight data using 5 bytes per row
  // Timestamp(int 2 bytes) - Status (char 1 byte) - Altitude (int 2 bytes)

  if (eepromAvalableBytes >= 5) {
    unsigned short currentEepromAddress = 1024 - eepromAvalableBytes;
    writeIntIntoEEPROM(currentEepromAddress, millisAtCurrentLoop / 10); // Addr n+0, n+1)
    if (status == 40) { // STATUS_LIFTOFF
      EEPROM.update(currentEepromAddress + 2, 'L'); // Addr n+2)
    } else if (status == 60) { // STATUS_APOGEE
      EEPROM.update(currentEepromAddress + 2, 'A'); // Addr n+2)
    } else if (status == 70) { // STATUS_EMERGENCY_DEPLOY
      EEPROM.update(currentEepromAddress + 2, 'E'); // Addr n+2)
    } else { // ANY OTHER 
      EEPROM.update(currentEepromAddress + 2, 'X'); // Addr n+2)
    }
    
    writeIntIntoEEPROM(currentEepromAddress + 3, currentLoopAltitude ); // Addr n+3, n+4)
    eepromAvalableBytes -= 5;
  }
}
#if SERIAL_DEBUG
void eepromDataReader() {
  for (int i = 0; i<= 1024; i+=5) {
    Serial.print(readIntFromEEPROM(i)); // timestamp 2 bytes
    Serial.print("\t");
    Serial.print((char) EEPROM.read(i+2)); // Status 1 byte
    Serial.print("\t");
    Serial.print(readIntFromEEPROM(i+3)); // Altitude 2 bytes
    Serial.println();
  }
}
#endif

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

#if SERIAL_DEBUG
 eepromDataReader();
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
  millisFromLastBlink = millisAtCurrentLoop;
}

void loop()
{
  // Unified time for the entire loop **millisAtCurrentLoop**
  if (millisAtLoopStart == 0) {
    millisAtLoopStart = millis();
  } else {
    millisAtCurrentLoop = millis() - millisAtLoopStart;
  }

  // Led status blink
  if (millisAtCurrentLoop - millisFromLastBlink >= 500)
  {
    statusMonitor();
    millisFromLastBlink = millisAtCurrentLoop;
  }

  // Skip all the above if its not ready or have landed
  if (status < STATUS_READY || status == STATUS_LANDED)
  {
    return;
  }
  rawTemp = bmp.readTemperature();
  rawAltitude = bmp.readAltitude(P0);
  currentLoopAltitude = KalmanCalc(rawAltitude) - initialAltitude;

#if SERIAL_DEBUG
  Serial.println("Runtime " + String(millisAtCurrentLoop));
  Serial.println("Raw Altitude " + String(rawAltitude));
  Serial.println("Kalman Altitude " + String(currentLoopAltitude));
  Serial.println("Previous Altitude: " + String(previousLoopAltitude));
  Serial.println("Raw Temp: " + String(rawTemp));
  Serial.println("Status: " + String(status));
#endif

  // Liftoff detection
  if ((currentLoopAltitude > initialAltitude + 1) && status == STATUS_READY)
  {
    status = STATUS_LIFTOFF;
    millisAtLiftoff = millisAtCurrentLoop;
  }

  // Apogee detection
  if (status == STATUS_LIFTOFF)
  {
    if (currentLoopAltitude > previousLoopAltitude)
    {
      consecutiveAltitudeDrop = APOGEE_THRESHOLD;
    }
    else
    {
      if (consecutiveAltitudeDrop == 0)
      {
        status = STATUS_APOGEE;
      }
      else
      {
        if (previousLoopAltitude != currentLoopAltitude)
        { 
          consecutiveAltitudeDrop -= 1;
        }
      }
    } 
    previousLoopAltitude = currentLoopAltitude;
  }

  // Deploy Parachute / Rescue secuence
  if (status == STATUS_APOGEE)
  {
    // Eject nose cone with servo
    deployParachute();
  }

  // Detect Landing
  // Check altitude to be repeating LANDED_THRESHOLD times on every LANDED_CHECK_INTERVAL milliseconds
  if (status >= STATUS_LIFTOFF && status < STATUS_LANDED)
  {
    // s
    // if (millisFromLastLandedCheck == 0) {
    //     millisFromLastLandedCheck = millisAtCurrentLoop;
    // }
    if (millisAtCurrentLoop - millisFromLastLandedCheck > LANDED_CHECK_INTERVAL) { 
      millisFromLastLandedCheck = millisAtCurrentLoop;

      if (landedCheckCounts == 0 || status == STATUS_EMERGENCY_DEPLOY) {
        status = STATUS_LANDED;
      } else {
        if (currentLoopAltitude == previousLoopAltitude)
        {
          landedCheckCounts -= 1;
        } else {
          landedCheckCounts = LANDED_THRESHOLD;
        }

      }
    }
  }

  if (status == STATUS_APOGEE || status == STATUS_EMERGENCY_DEPLOY)
  {
    if (abs(currentLoopAltitude - initialAltitude) < 5)
    {
      status = STATUS_LANDED;
    }
  }

  // Emergency parachute deployment
  if (status == STATUS_LIFTOFF and millisAtLiftoff > 0 and (millisAtCurrentLoop - millisAtLiftoff) >= SECURITY_DEPLOYMENT_TIME)
  {
    deployParachute();
    status = STATUS_EMERGENCY_DEPLOY;
  }

  // SD storage on whenever ready
  // code as is will only write 1 last log when landed
  sdDataLogger();
  if (status >= STATUS_LIFTOFF and status < STATUS_LANDED)
  {
    // To prevent damaging eeprom, only store important flight data 
    eepromDataLogger();
  }
}
