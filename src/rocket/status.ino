void statusMonitor() {
  if (allOn == true) {
    allOn = false;
    if (liftoff == true)
    {
      digitalWrite(greenLed, HIGH);
    }
    if (apogeeHasFired == true)
    {
      tone(buzzer, 440);
      digitalWrite(blueLed, HIGH);
    }
    if (err == true) {
      digitalWrite(redLed, HIGH);
      digitalWrite(blueLed, HIGH);
      digitalWrite(greenLed, HIGH);
      tone(buzzer, 440);
    }
    if (landed == true)
    {
      //playNeverGonnaGiveYouUp();
      tone(buzzer, 440);
      digitalWrite(redLed, HIGH);
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
  for (int j = 0; j < 3; j++) {
    digitalWrite(redLed, HIGH);
    delay(100);
    digitalWrite(redLed, LOW);
    delay(100);
    digitalWrite(greenLed, HIGH);
    delay(100);
    digitalWrite(greenLed, LOW);
    delay(100);
    digitalWrite(blueLed, HIGH);
    delay(100);
    digitalWrite(blueLed, LOW);
    delay(500);
  }
}
