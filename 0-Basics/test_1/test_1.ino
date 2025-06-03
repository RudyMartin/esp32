#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


// --- Pin Configuration ---
const uint8_t SERVO_SDA   = 6;
const uint8_t SERVO_SCL   = 7;

#define SERVOMIN 102
#define SERVOMAX 512

void setup() {
  Serial.begin(115200);
  Wire.begin(6, 7);  // SDA = 6, SCL = 7
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(10);
  Serial.println("Testing servo on CH0...");

  pwm.setPWM(0, 0, SERVOMIN); delay(1000);
  pwm.setPWM(0, 0, SERVOMAX); delay(1000);
  pwm.setPWM(0, 0, (SERVOMIN + SERVOMAX)/2);
}

void loop() {}
