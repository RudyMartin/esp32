/**********************************************************************
 * File       : c_servo_speed_test.ino
 * Title      : Servo Menu + Full Sweep Test with Interactive Control
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: Menu to test GPIO or PCA9685 servos with live controls,
 *              speed settings, and smooth motion.
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

// === Speed State ===
int stepDelay = 15;                 // Default = medium
String currentSpeed = "medium";     // For display

void setSpeed(String level) {
  level.toLowerCase();
  if (level == "fast") {
    stepDelay = 5;
    currentSpeed = "fast";
    Serial.println("⚡ Speed set to FAST");
  } else if (level == "medium") {
    stepDelay = 15;
    currentSpeed = "medium";
    Serial.println("🚗 Speed set to MEDIUM");
  } else if (level == "slow") {
    stepDelay = 30;
    currentSpeed = "slow";
    Serial.println("🐢 Speed set to SLOW");
  } else {
    Serial.println("❌ Invalid speed. Use: slow, medium, fast");
  }
}

// === Menu ===
void showMenu() {
  Serial.println("\n====== Servo Test Menu ======");
  Serial.println("1. GPIO Servo Sweep (SG92R on GATE_SERVO_PIN)");
  Serial.println("2. PCA9685 Full Sweep (6-DOF Robot Arm)");
  Serial.println("3. Gate Servo – Interactive Angle (GPIO)");
  Serial.println("4. Aux Servo – Interactive Angle (PCA9685 CH7)");
  Serial.println("Enter your choice (1–4): ");
}

// === Smooth Servo Movement ===
void smoothMoveGPIO(int targetAngle) {
  int current = gpioServo.read();
  int step = (targetAngle > current) ? 1 : -1;
  Serial.printf("🟢 Moving → %d° at SPEED=%s\n", targetAngle, currentSpeed.c_str());
  for (int pos = current; pos != targetAngle; pos += step) {
    gpioServo.write(pos);
    delay(stepDelay);
  }
  gpioServo.write(targetAngle);
}

void smoothMovePCA(int channel, int targetAngle, const ServoSpecs& specs) {
  int currentAngle = specs.minAngle;  // Starting assumption
  int step = (targetAngle > currentAngle) ? 1 : -1;
  Serial.printf("🟢 Moving → %d° at SPEED=%s\n", targetAngle, currentSpeed.c_str());
  for (int angle = currentAngle; angle != targetAngle; angle += step) {
    int pulse = map(angle, specs.minAngle, specs.maxAngle, specs.minPulse, specs.maxPulse);
    pwm.setPWM(channel, 0, pulse);
    delay(stepDelay);
  }
  int finalPulse = map(targetAngle, specs.minAngle, specs.maxAngle, specs.minPulse, specs.maxPulse);
  pwm.setPWM(channel, 0, finalPulse);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.printf("\n🎓 Hello %s! Welcome to Servo Tester.\n", username.c_str());

  while (mode == 0) {
    showMenu();
    while (!Serial.available()) delay(100);
    String input = Serial.readStringUntil('\n');
    input.trim();
    mode = input.toInt();
    if (mode < 1 || mode > 4) {
      Serial.println("❌ Invalid choice. Please enter 1–4.");
      mode = 0;
    }
  }

  if (mode == 1 || mode == 3) {
    gpioServo.attach(SERVO_GATE_PIN);
    Serial.println("🔧 GPIO Servo initialized.");
  }

  if (mode == 2 || mode == 4) {
    pwm.begin();
    pwm.setPWMFreq(50);
    delay(500);
    Serial.println("🔧 PCA9685 initialized.");
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
  } 
  else if (mode == 2) {
    testServo(SERVO_BASE_CHANNEL,         SERVO_BASE_MIN_ANGLE,         SERVO_BASE_MAX_ANGLE,         SERVO_BASE_TYPE);
    testServo(SERVO_SHOULDER_CHANNEL,     SERVO_SHOULDER_MIN_ANGLE,     SERVO_SHOULDER_MAX_ANGLE,     SERVO_SHOULDER_TYPE);
    testServo(SERVO_ELBOW_CHANNEL,        SERVO_ELBOW_MIN_ANGLE,        SERVO_ELBOW_MAX_ANGLE,        SERVO_ELBOW_TYPE);
    testServo(SERVO_WRIST_TILT_CHANNEL,   SERVO_WRIST_TILT_MIN_ANGLE,   SERVO_WRIST_TILT_MAX_ANGLE,   SERVO_WRIST_TILT_TYPE);
    testServo(SERVO_WRIST_ROTATE_CHANNEL, SERVO_WRIST_ROTATE_MIN_ANGLE, SERVO_WRIST_ROTATE_MAX_ANGLE, SERVO_WRIST_ROTATE_TYPE);
    testServo(SERVO_GRIPPER_CHANNEL,      SERVO_GRIPPER_MIN_ANGLE,      SERVO_GRIPPER_MAX_ANGLE,      SERVO_GRIPPER_TYPE);
    delay(2000);
  } 
  else if (mode == 3 || mode == 4) {
    Serial.println("🎛️ Interactive Angle Control");
    Serial.println("📏 Type: angle=VALUE (0–180)");
    Serial.println("🎚️ Type: speed=slow | medium | fast");
    Serial.println("❌ Type: quit       (to return to menu)");
    Serial.println("🧪 Or combine: angle=90 speed=slow");

    // Auto-center on entry
    if (mode == 3) smoothMoveGPIO(90);
    else if (mode == 4) smoothMovePCA(SERVO_AUX_CHANNEL, 90, SERVO_AUX_TYPE);

    while (true) {
      if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        input.toLowerCase();

        if (input.equalsIgnoreCase("quit")) {
          Serial.println("👋 Exiting to main menu...");
          if (mode == 3) smoothMoveGPIO(90);
          else if (mode == 4) smoothMovePCA(SERVO_AUX_CHANNEL, 90, SERVO_AUX_TYPE);
          mode = 0;
          setup();
          return;
        }

        // Parse angle and speed from combined input
        int angleVal = -1;
        String speedVal = "";
        int sep = input.indexOf(' ');
        while (sep != -1) {
          String part = input.substring(0, sep);
          input = input.substring(sep + 1);
          if (part.startsWith("angle=")) angleVal = part.substring(6).toInt();
          else if (part.startsWith("speed=")) speedVal = part.substring(6);
          sep = input.indexOf(' ');
        }
        if (input.startsWith("angle=")) angleVal = input.substring(6).toInt();
        else if (input.startsWith("speed=")) speedVal = input.substring(6);

        if (speedVal.length() > 0) setSpeed(speedVal);
        if (angleVal >= 0 && angleVal <= 180) {
          if (mode == 3) smoothMoveGPIO(angleVal);
          else if (mode == 4) smoothMovePCA(SERVO_AUX_CHANNEL, angleVal, SERVO_AUX_TYPE);
        } else if (angleVal != -1) {
          Serial.println("❌ Invalid angle. Must be 0–180.");
        }

        if (angleVal == -1 && speedVal == "") {
          Serial.println("💡 Commands:");
          Serial.println("  angle=VALUE (0–180)");
          Serial.println("  speed=slow | medium | fast");
          Serial.println("  quit");
          Serial.println("  Or combine: angle=120 speed=fast");
        }
      }
    }
  }
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
