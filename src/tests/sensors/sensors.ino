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
#include <Adafruit_BMP085.h>
#include <MPU6050.h>

MPU6050 mpu;
Adafruit_BMP085 bmp;
int ax, ay, az, gx, gy, gz;
float P0;

bool useMPUData = true;
int temp, altitudeInt, millisAtRead = 0;
float altitudeFloat;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  if (!bmp.begin(10))
  {
    Serial.println("BMP sensor failed!");
  }

  // MPU Sensor Initialisation
  mpu.initialize();
  if (!mpu.testConnection())
  {
    Serial.println("MPU Failed!");
    useMPUData = false;
  }

  float pressoureAccum = 0;
  for (int i = 0; i < 10; i++)
  {
    delay(10);
    pressoureAccum += bmp.readPressure();
  }

  P0 = pressoureAccum / 10;

}

void loop()
{
  millisAtRead = millis();
  temp = bmp.readTemperature();
  altitudeFloat = bmp.readAltitude(P0);
  altitudeInt = altitudeInt;
  

  Serial.print(millis() - millisAtRead);
  Serial.print("\t");
  Serial.print(temp);
  Serial.print("\t");
  Serial.print(altitudeInt);
  Serial.print("\t");
  Serial.print(altitudeFloat);
  Serial.print("\t");
  if (useMPUData == true)
  {
    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);
    Serial.print(ax / 2048);
    Serial.print("\t");
    Serial.print(ay / 2048);
    Serial.print("\t");
    Serial.print(az / 2048);
    Serial.print("\t");
    Serial.print(gx);
    Serial.print("\t");
    Serial.print(gy);
    Serial.print("\t");
    Serial.print(gz);
  }
  Serial.println();
  delay(200);

}
