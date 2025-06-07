/**********************************************************************
* Filename    : sensor_utils.h
* Description : Simple abstraction for reading analog sensors
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include <Arduino.h>

// Reads and returns a normalized voltage value from analog sensor
float readAnalogSensor(int pin) {
  int raw = analogRead(pin);
  float voltage = raw * (3.3 / 4095.0); // ESP32 ADC is 12-bit
  return voltage;
}

#endif