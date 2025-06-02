/**********************************************************************
 * File       : c_servo_test.ino
 * Title      : Servo Menu + Full Sweep Test (with Timeout)
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

// === Menu Display ===
void showMenu() {
  Serial.println("\n====== Servo Test Menu ======");
  Serial.println("1. GPIO Servo Test (SG92R on GATE_SERVO_PIN)");
  Serial.println("2. PCA9685 Full Sweep (6-DOF Robot Arm)");
  Serial.println("Enter your choice (1–2): ");
  Serial.println("⌛ Waiting 10 seconds for input... default = 1");
}

// === Servo Test (PCA9685)
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

// === Setup with Timeout Menu ===
void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.printf("\n🎓 Hello %s! Welcome to Servo Tester.\n", username.c_str());

  unsigned long startTime = millis();
  bool inputReceived = false;
  showMenu();

  while ((millis() - startTime) < 10000) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      int selection = input.toInt();
      if (selection >= 1 && selection <= 2) {
        mode = selection;
        inputReceived = true;
        Serial.printf("✅ Selected mode %d\n", mode);
        break;
      } else {
        Serial.println("❌ Invalid input. Try again.");
        showMenu();
        startTime = millis();  // reset timeout window
      }
    }
    delay(100);
  }

  if (!inputReceived) {
    mode = 2;
    Serial.println("⏱️ No input received. Defaulting to mode 2 (PCA9685 Full Sweep)");
  }

  // Initialize selected mode
  if (mode == 1) {
    Serial.println("🔧 GPIO Servo Test Selected");
    gpioServo.attach(SERVO_GATE_PIN);
  } else if (mode == 2) {
    Serial.println("Entering Default Mode");
    Serial.println("🔧 PCA9685 Full Sweep Test Selected");
    pwm.begin();
    pwm.setPWMFreq(50);  // Standard analog servo frequency
    delay(500);
  }
}

// === Loop: Run Mode
void loop() {
  if (mode == 1) {
    gpioServo.write(0);
    Serial.println("[GPIO Servo] → 0°");
    delay(1000);
    gpioServo.write(180);
    Serial.println("[GPIO Servo] → 180°");
    delay(1000);
  } 
  else if (mode == 2) {
    testServo(SERVO_BASE_CHANNEL,         SERVO_BASE_MIN_ANGLE,         SERVO_BASE_MAX_ANGLE,         SERVO_BASE_TYPE);
    testServo(SERVO_SHOULDER_CHANNEL,     SERVO_SHOULDER_MIN_ANGLE,     SERVO_SHOULDER_MAX_ANGLE,     SERVO_SHOULDER_TYPE);
    testServo(SERVO_ELBOW_CHANNEL,        SERVO_ELBOW_MIN_ANGLE,        SERVO_ELBOW_MAX_ANGLE,        SERVO_ELBOW_TYPE);
    testServo(SERVO_WRIST_TILT_CHANNEL,   SERVO_WRIST_TILT_MIN_ANGLE,   SERVO_WRIST_TILT_MAX_ANGLE,   SERVO_WRIST_TILT_TYPE);
    testServo(SERVO_WRIST_ROTATE_CHANNEL, SERVO_WRIST_ROTATE_MIN_ANGLE, SERVO_WRIST_ROTATE_MAX_ANGLE, SERVO_WRIST_ROTATE_TYPE);
    testServo(SERVO_GRIPPER_CHANNEL,      SERVO_GRIPPER_MIN_ANGLE,      SERVO_GRIPPER_MAX_ANGLE,      SERVO_GRIPPER_TYPE);

    delay(2000); // Pause before repeating
  }
}
