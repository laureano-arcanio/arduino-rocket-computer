  void dataLogger()
{
  String logLine = String (initialAltitude) + ", "+ String (currAltitude) + ", " + String (rawAltitude) + ", " +  String (millis()) + ", " + String (status);
  
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(logLine);
    dataFile.close();
  }
}
