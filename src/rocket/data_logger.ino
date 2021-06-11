void dataLogger()
{
  mpu.getAcceleration(&ax, &ay, &az);
  float aX = ax / 2048;
  float aY = ay / 2048;
  float aZ = az / 2048;

  String logLine = String (currAltitude) + ", " + String (rawAltitude) + ", " + String (sqrt(aX * aX + aY * aY + aZ * aZ)) + ", " +  String (millis()) + ", " + String (status);
  
  File dataFile = SD.open("logs.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(logLine);
    dataFile.close();
  }
}
