/**********************************************************************
 * File       : pca_test.ino
 * Title      : PCA9685 Connection Test (Skinny Version)
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: Verifies I2C communication with PCA9685 via GPIO 4/5
 **********************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Restate Pin Config for your board
static const uint8_t SDA = 8;
static const uint8_t SCL = 9;

void waitForSerial() {
  unsigned long start = millis();
  while (!Serial && millis() - start < 10000) {
    delay(100);
  }

  // Extra wait for host to sync
  delay(1000);
  Serial.println("📡 Serial connected. Waiting to stabilize...");
  delay(1000);

  Serial.println("✅ Serial is alive. Type anything to continue...");
  while (!Serial.available()) {
    delay(100);
  }
  Serial.read();  // consume input
  Serial.println("📨 Received input. Continuing setup...");
}

void setup() {
  Serial.begin(115200);
  waitForSerial();

  Serial.println("🔌 Starting PCA9685 I2C Test");

  // Restate PIN Config for your board
  Wire.begin(8, 9);           // SDA = GPIO 4, SCL = GPIO 5
  Wire.setClock(400000);     // Optional: 400kHz for faster I2C

  pwm.begin();
  pwm.setPWMFreq(50);
  delay(100);

  Serial.println("✅ PCA9685 Initialized. Sending test pulse on channel 0...");
  pwm.setPWM(0, 0, 300);      // Send low pulse
  delay(500);
  pwm.setPWM(0, 0, 500);      // Send mid pulse
  delay(500);
  pwm.setPWM(0, 0, 400);      // Center
  Serial.println("🎯 Test pulse sent. Check servo response or monitor for I2C errors.");
}

int loopCount = 0;

void loop() {
  if (loopCount < 5) {
    Serial.printf("\n");
    Serial.printf("🔁 Loop iteration #%d\n", loopCount + 1);
    loopCount++;
    delay(1000);  // Optional: slow down for readability
  }
}



