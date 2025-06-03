/**********************************************************************
 * File       : c_servo_pca9685_test.ino
 * Title      : PCA9685 Servo Sweep Test with I2C Detection
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: Scans I2C, initializes PCA9685, and sends test pulses
 *              to all 16 channels with verbose feedback.
 **********************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const uint8_t SERVO_SDA = 4;
const uint8_t SERVO_SCL = 5;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

void waitForSerial();
void scanI2CDevices(TwoWire &bus);
void testAllServoChannels();

void setup() {
  Serial.begin(115200);
  waitForSerial();

  Serial.println("🔌 Starting PCA9685 Servo Detection Sketch");

  Wire.begin(SERVO_SDA, SERVO_SCL);
  Wire.setClock(400000);

  scanI2CDevices(Wire);

  Serial.println("⚙️  Initializing PCA9685...");
  pwm.begin();
  pwm.setPWMFreq(50);  // 50 Hz for standard servos
  delay(100);
  Serial.println("✅ PCA9685 ready. Beginning channel scan...");

  testAllServoChannels();
}

void loop() {
  // Freeze after test, or do it once per reset
}

/* ---------------------------------------------
 * Wait for serial connection to stabilize
 * --------------------------------------------- */
void waitForSerial() {
  unsigned long start = millis();
  while (!Serial && millis() - start < 10000) {
    delay(100);
  }
  delay(1000);
  Serial.println("📡 Serial connected. Waiting to stabilize...");
  delay(1000);
}

/* ---------------------------------------------
 * Verbose I2C scanner with PCA9685 signature
 * --------------------------------------------- */
void scanI2CDevices(TwoWire &bus) {
  Serial.println("🔍 Scanning I2C bus for devices...");

  byte error, address;
  int nDevices = 0;
  bool pcaFound = false;

  for (address = 1; address < 127; address++) {
    bus.beginTransmission(address);
    error = bus.endTransmission();

    if (error == 0) {
      Serial.print("✅ I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);

      // Try to read MODE1 register (0x00)
      bus.beginTransmission(address);
      bus.write(0x00);  // MODE1
      if (bus.endTransmission(false) == 0 && bus.requestFrom(address, (uint8_t)1)) {
        byte mode1 = bus.read();
        Serial.print(" | MODE1: 0x");
        Serial.print(mode1, HEX);

        if (mode1
