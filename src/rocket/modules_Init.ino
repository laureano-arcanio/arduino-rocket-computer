void SDcardInit()
{
  //SD card init
  if (!SD.begin(chipSelect)) { // see if the card is present and can be initialized:
    //Serial.println("Card failed, or not present");
    status = 10;
  }
}

void bmpInit()
{
  if (!bmp.begin()) //Presure Sensor Initialisation
  {
    //Serial.println("BMP sensor failed!");
    status = 11;
  }
}


void mpuInit()
{
  //Accel/gyro Sensor Initialisation
  mpu.initialize(); //Accel/gyro Sensor Initialisation
  if (!mpu.testConnection()) //Test Connection
  {
    //Serial.println("MPU Failed!");
    status = 12;
  }
}


