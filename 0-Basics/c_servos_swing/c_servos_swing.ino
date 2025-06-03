/**********************************************************************
 * File       : c_servos_swing.ino
 * Title      : PCA9685 Servo Sweep + I2C Scan (20 Loops Max)
 * Author     : Rudy Martin / Next Shift Consulting
 * Description: Modular swing test with persistent I2C address output.
 * Status.    : Runs throughy loop no errors / no servo moves
 **********************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- Pin Configuration ---
const uint8_t SDA_PIN = 6;
const uint8_t SCL_PIN = 7;
const uint8_t NUM_SERVOS = 8;
const uint16_t SERVO_FREQ = 50;  // Hz

// --- PWM Driver & I2C ---
uint8_t foundPCA9685Addr = 0;
Adafruit_PWMServoDriver pwm;
int loopCount = 0;
const int maxLoops = 20;

// --- Angle Mapping (0°–180° to pulse)
uint16_t angleToPulse(int angle) {
  return map(angle, 0, 180, 102, 512);  // Typical range for SG90
}

// --- I2C Scanner ---
void scanAndConfirmPCA9685Address(TwoWire &bus) {
  Serial.println("🔍 Scanning I2C bus for PCA9685...");
  for (uint8_t addr = 1; addr < 127; addr++) {
    bus.beginTransmission(addr);
    if (bus.endTransmission() == 0) {
      Serial.printf("✅ Device found at 0x%02X\n", addr);
      if (foundPCA9685Addr == 0) {
        foundPCA9685Addr = addr;
      }
    }
  }

  if (foundPCA9685Addr > 0) {
    Serial.printf("🎯 PCA9685 detected at address: 0x%02X\n", foundPCA9685Addr);
    pwm = Adafruit_PWMServoDriver(foundPCA9685Addr, Wire);
    pwm.begin();
    pwm.setPWMFreq(SERVO_FREQ);
    delay(100);
  } else {
    Serial.println("⚠️ PCA9685 not found. Will continue scanning...");
  }
}

// --- Core MCP Agent Entry ---
void runSelectedRoutine() {
  runRoutine_sweep();
}

// --- Test Routine: Sweep All Servos ---
void runRoutine_sweep() {
  for (int ch = 0; ch < NUM_SERVOS; ch++) {
    Serial.printf("🔁 Sweeping servo %d\n", ch);
    moveToAngle(ch, 30);
    delay(400);
    moveToAngle(ch, 120);
    delay(400);
    moveToAngle(ch, 0);
    delay(300);
  }
  Serial.println("✅ Sweep complete.");
}

// --- Move Servo to Angle ---
void moveToAngle(uint8_t ch, int angle) {
  uint16_t pulse = angleToPulse(angle);
  pwm.setPWM(ch, 0, pulse);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("🔌 Starting PCA9685 Swing Test");

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);
  scanAndConfirmPCA9685Address(Wire);
}

// --- Loop with Max Counter ---
void loop() {
  if (loopCount >= maxLoops) {
    Serial.println("🛑 Max loop count reached. Halting loop.");
    while (true);  // stop here
  }

  Serial.printf("🔁 Loop iteration %d of %d\n", loopCount + 1, maxLoops);

  if (foundPCA9685Addr > 0) {
    Serial.printf("🎯 Confirmed PCA9685 at 0x%02X – Running routine.\n", foundPCA9685Addr);
    runSelectedRoutine();
  } else {
    Serial.println("🚫 No PCA9685 detected – rescanning...");
    scanAndConfirmPCA9685Address(Wire);
  }

  loopCount++;
  delay(5000);
}


