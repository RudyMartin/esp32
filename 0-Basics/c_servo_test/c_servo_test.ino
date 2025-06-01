// b_servo_test.ino – Test Servos and Stepper Motor
// Author: Rudy Martin / Next Shift Consulting
// Project: Artemis DSAI 2025 Camp

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP32Servo.h>

// === USER CONFIG ===
// do not use # we created a b_stepper_motor_test.ino for that
int test_mode = 3;  // 1 = GPIO Servo, 2 = PCA9685 Servo, 3 = Stepper Motor
String username = "student";

// === GPIO Servo Setup (test_mode = 1) ===
Servo myServo;
int servoPin = 10;

// === PCA9685 Setup (test_mode = 2) ===
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
int pcaChannel = 0;
int servoMin = 130;  // Min pulse length out of 4096
int servoMax = 500;  // Max pulse length out of 4096

// === Stepper Setup (test_mode = 3) ===
int IN1 = 11;  // ULN2003 IN1 → GPIO 11
int IN2 = 12;  // IN2 → GPIO 12
int IN3 = 13;  // IN3 → GPIO 13
int IN4 = 14;  // IN4 → GPIO 14
int stepDelay = 4; // milliseconds between steps

int stepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.printf("\n🛠️ Hello %s, welcome to the Servo/Stepper Test!\n", username.c_str());

  if (test_mode == 1) {
    Serial.println("🔧 Mode 1: GPIO Servo Test");
    myServo.attach(servoPin);
  } else if (test_mode == 2) {
    Serial.println("🔧 Mode 2: PCA9685 Servo Test");
    pwm.begin();
    pwm.setPWMFreq(50);
  } else if (test_mode == 3) {
    Serial.println("🔧 Mode 3: Stepper Test on ULN2003");
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
  }
}

void loop() {
  if (test_mode == 1) {
    Serial.println("[GPIO Servo] 0° → 180°");
    myServo.write(0);
    delay(1000);
    myServo.write(180);
    delay(1000);
  } else if (test_mode == 2) {
    Serial.println("[PCA Servo] Min → Max");
    pwm.setPWM(pcaChannel, 0, servoMin);
    delay(1000);
    pwm.setPWM(pcaChannel, 0, servoMax);
    delay(1000);
  } else if (test_mode == 3) {
    Serial.println("[Stepper] Rotating CW 512 steps");
    for (int i = 0; i < 512; i++) {
      for (int step = 0; step < 8; step++) {
        digitalWrite(IN1, stepSequence[step][0]);
        digitalWrite(IN2, stepSequence[step][1]);
        digitalWrite(IN3, stepSequence[step][2]);
        digitalWrite(IN4, stepSequence[step][3]);
        delay(stepDelay);
      }
    }
    delay(1000);
  }
}
