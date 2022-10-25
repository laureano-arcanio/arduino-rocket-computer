
boolean hasHeaders = false;
 
void dataLogger() {
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);
  // CSV format:
  // Millis from init, Status, Initial Altitude, Current Altitude (by kalman), Raw Altitude measurement, Acc X, Acc Y, Acc Z, Gyro X, Gyro Y, Giro Z
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {

    if ( hasHeaders == false) {
      //dataFile.println("Millis\tStatus Code\tInitial Altitude\tCurrent Altitud\tMeasured Altitude\tAcc X\tAcc Y\tAcc Z\tGyro X\tGyro Y\tGyro Z");
      
      dataFile.println("Ms\tCode\tA0\tCA\tMA\tAX\tAY\tAZ\tGX\tGY\tGZ");
      hasHeaders = true;
    }
    dataFile.print(millis()); dataFile.print("\t");
    dataFile.print(status); dataFile.print("\t");
    dataFile.print(initialAltitude); dataFile.print("\t");
    dataFile.print(currAltitude); dataFile.print("\t");
    dataFile.print(rawAltitude); dataFile.print("\t");
    dataFile.print(ax / 2048); dataFile.print("\t");
    dataFile.print(ay / 2048); dataFile.print("\t");
    dataFile.print(az / 2048); dataFile.print("\t");
    dataFile.print(gx); dataFile.print("\t");
    dataFile.print(gy); dataFile.print("\t");
    dataFile.print(gz); dataFile.print("\t");
    dataFile.print("\n");
    dataFile.close();
  }
}