/**********************************************************************
* Filename    : button_utils.h
* Description : Utility for debounced button press detection
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#ifndef BUTTON_UTILS_H
#define BUTTON_UTILS_H

#include <Arduino.h>

struct Button {
  int pin;
  bool lastState;
  unsigned long lastDebounceTime;
  unsigned long debounceDelay;
};

void setupButton(Button &btn, int pin, unsigned long debounceDelay = 50) {
  btn.pin = pin;
  btn.lastState = HIGH;
  btn.lastDebounceTime = 0;
  btn.debounceDelay = debounceDelay;
  pinMode(pin, INPUT_PULLUP);
}

bool isButtonPressed(Button &btn) {
  bool reading = digitalRead(btn.pin);
  unsigned long now = millis();

  if (reading != btn.lastState) {
    btn.lastDebounceTime = now;
  }

  if ((now - btn.lastDebounceTime) > btn.debounceDelay) {
    if (reading == LOW && btn.lastState == HIGH) {
      btn.lastState = reading;
      return true;  // Button press detected
    }
  }

  btn.lastState = reading;
  return false;
}

#endif
