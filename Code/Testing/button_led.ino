// ------------------------------------------------------------
// Code to validate wiring and functionality of button and LED
// Pushing button will toggle LED on/off
// ------------------------------------------------------------

#define led_pin 13
#define button_pin 12

bool isLedOn = false;
int prevButtonState = HIGH; // prevents led toggling if button was held down

void setup() {
  pinMode(button_pin, INPUT_PULLUP); // use internal pull-up for button, reverses button state
  pinMode(13, OUTPUT);
}

void loop() {
  int buttonState = digitalRead(button_pin);

  if (buttonState == LOW && prevButtonState == HIGH && isLedOn == false) // button pressed and led is off
  {
    digitalWrite(led_pin, HIGH);
    isLedOn = true;
  }
  else if (buttonState == LOW && prevButtonState == HIGH && isLedOn == true) // button pressed and led is on
  {
    digitalWrite(13, LOW);
    isLedOn = false;
  }

  prevButtonState = buttonState;
  delay(200); // debounce button
}