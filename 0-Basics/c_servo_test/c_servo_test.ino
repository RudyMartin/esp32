/**********************************************************************
 * File       : c_servo_test.ino
 * Title      : Servo Menu + Full Sweep Test
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: Menu to test either a GPIO-based servo or sweep all
 *              PCA9685 servos using SG92R specs.
 **********************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP32Servo.h>

#include "project_config.h"
#include "servo_config.h"

// === Setup ===
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Servo gpioServo;
String username = "student";
int mode = 0;

// === Menu ===
void showMenu() {
  Serial.println("\n====== Servo Test Menu ======");
  Serial.println("1. GPIO Servo Test (SG92R on GATE_SERVO_PIN)");
  Serial.println("2. PCA9685 Full Sweep (6-DOF Robot Arm)");
  Serial.println("Enter your choice (1–2): ");
}

// === PCA Sweep Helper ===
void testServo(int channel, int minAngle, int maxAngle, const ServoSpecs& specs) {
  Serial.printf("🎯 Testing channel %d: %d° to %d°\n", channel, minAngle, maxAngle);
  int minPulse = specs.minPulse;
  int maxPulse = specs.maxPulse;

  for (int angle = minAngle; angle <= maxAngle; angle += 10) {
    int pulse = map(angle, specs.minAngle, specs.maxAngle, minPulse, maxPulse);
    pwm.setPWM(channel, 0, pulse);
    delay(300);
  }

  // Return to center
  int centerPulse = map((minAngle + maxAngle) / 2, specs.minAngle, specs.maxAngle, minPulse, maxPulse);
  pwm.setPWM(channel, 0, centerPulse);
  delay(500);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.printf("\n🎓 Hello %s! Welcome to Servo Tester.\n", username.c_str());

  showMenu();
  while (mode == 0) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      mode = input.toInt();
      if (mode < 1 || mode > 2) {
        Serial.println("❌ Invalid choice. Please enter 1 or 2.");
        mode = 0;
        showMenu();
      }
    }
  }

  if (mode == 1) {
    Serial.println("🔧 GPIO Servo Test Selected");
    gpioServo.attach(SERVO_GATE_PIN);  // e.g. SG92R
  } else if (mode == 2) {
    Serial.println("🔧 PCA9685 Full Sweep Test Selected");
    pwm.begin();
    pwm.setPWMFreq(50); // Standard analog servo freq
    delay(500);
  }
}

void loop() {
  if (mode == 1) {
    gpioServo.write(0);
    Serial.println("[GPIO Servo] → 0°");
    delay(1000);
    gpioServo.write(180);
    Serial.println("[GPIO Servo] → 180°");
    delay(1000);
  } else if (mode == 2) {
    // Sweep all configured PCA channels
    testServo(SERVO_BASE_CHANNEL,         SERVO_BASE_MIN_ANGLE,         SERVO_BASE_MAX_ANGLE,         SERVO_BASE_TYPE);
    testServo(SERVO_SHOULDER_CHANNEL,     SERVO_SHOULDER_MIN_ANGLE,     SERVO_SHOULDER_MAX_ANGLE,     SERVO_SHOULDER_TYPE);
    testServo(SERVO_ELBOW_CHANNEL,        SERVO_ELBOW_MIN_ANGLE,        SERVO_ELBOW_MAX_ANGLE,        SERVO_ELBOW_TYPE);
    testServo(SERVO_WRIST_TILT_CHANNEL,   SERVO_WRIST_TILT_MIN_ANGLE,   SERVO_WRIST_TILT_MAX_ANGLE,   SERVO_WRIST_TILT_TYPE);
    testServo(SERVO_WRIST_ROTATE_CHANNEL, SERVO_WRIST_ROTATE_MIN_ANGLE, SERVO_WRIST_ROTATE_MAX_ANGLE, SERVO_WRIST_ROTATE_TYPE);
    testServo(SERVO_GRIPPER_CHANNEL,      SERVO_GRIPPER_MIN_ANGLE,      SERVO_GRIPPER_MAX_ANGLE,      SERVO_GRIPPER_TYPE);

    delay(2000); // Pause before repeating
  }
}
