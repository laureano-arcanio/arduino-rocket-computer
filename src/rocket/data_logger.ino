void dataLogger()
{
  if (liftoff == true)
  {
    //Accel calcs
    mpu.getAcceleration(&ax, &ay, &az);
    float aX = ax / 2048; float aY = ay / 2048; float aZ = az / 2048;
    float at = sqrt(aX * aX + aY * aY + aZ * aZ); // RAW total accel of axis
    /* OTHER DATA
      // String pressure = String (bmp.readPressure()); // pressure in pascals
      // String temp = String (bmp.readTemperature()); //temperature in degrees
    */
    String aT = String (at);
    String comma = String ("  ,  ");
    String alt = String (currAltitude);
    String rawAlt = String (rawAltitude);
    //Time Stamp String
    String timer = String (millis());

    //make a big string containing above strings
    String Baro_data = String (("Altitude= " + alt) + comma + ("Raw= " + rawAlt) + comma + ("accel= " + aT) + comma + timer);

    //String flight_Data = String (alt + comma + at + comma + timer);
    //String all_data = String (("Altitude= " + alt + " m") + comma + ("Pressure= " + pressure + " Pa") + comma + ("Temp = " + temp + " °C") + comma + ("aX= " + accelX + " g") + comma + timer) ;

    //  Serial.println (Baro_data); // DEGUB

    File dataFile = SD.open("dataLog.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(Baro_data); //put Baro_data on the SD card & print apogee altitude to SD Card
      dataFile.close();
    }
  }
}
