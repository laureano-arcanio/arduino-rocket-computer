void statusMonitor() {
  //Serial.println(status);
  if (allOn == true) {
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
      //playNeverGonnaGiveYouUp();
    }
    if (status == STATUS_EMERGENCY_DEPLOY)
    {
      digitalWrite(redLed, HIGH);
      digitalWrite(blueLed, HIGH);
      digitalWrite(greenLed, HIGH);
      //playNeverGonnaGiveYouUp();
    }
    
    // Error codes ar coded 10 to 19
    if (status < 20) {
      // 1x Errors
      // 10 = SD Error
      // 11 = BMP Error
      // 12 = MPU
      if (status == 10 ) {
        digitalWrite(redLed, HIGH);
        digitalWrite(greenLed, HIGH);
      }
      if (status == 11 ) {
        digitalWrite(redLed, HIGH);
        digitalWrite(blueLed, HIGH);
      }
      if (status == 12 ) {

        // this is not used when MPU not present
        digitalWrite(LED_BUILTIN, HIGH);
      }
      tone(buzzer, 880);
    }
  }
  else 
  {
    allOn = true;
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    noTone(buzzer);
  }
} 
