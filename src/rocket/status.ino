void statusMonitor() {
  if (allOn == true) {
    allOn = false;
    if (status == STATUS_LIFTOFF)
    {
      digitalWrite(greenLed, HIGH);
      tone(buzzer, 587);
    }
    if (status == STATUS_APOGEE)
    {
      tone(buzzer, 440);
      digitalWrite(blueLed, HIGH);
    }
    if (status == STATUS_LANDED)
    {
      //playNeverGonnaGiveYouUp();
      tone(buzzer, 880);
      digitalWrite(redLed, HIGH);
    }
    // Error codes ar coded 10 to 19
    if (status < 20) {
      digitalWrite(redLed, HIGH);
      digitalWrite(blueLed, HIGH);
      digitalWrite(greenLed, HIGH);
      tone(buzzer, 440);
    }
  }
  else 
  {
    allOn = true;
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
    noTone(buzzer);
  }
}

void err_Led() {
  digitalWrite(redLed, HIGH); //RED LED
}

void init_Led() {
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, HIGH);
  digitalWrite(blueLed, HIGH);
  tone(buzzer, 440);
  delay(2000);
  noTone(buzzer);
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);
}
