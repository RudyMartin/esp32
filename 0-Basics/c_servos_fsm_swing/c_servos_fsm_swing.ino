/**********************************************************************
 * File       : c_servos_fsm_swing.ino
 * Title      : Modular Servo Swing Framework (8 Servos)
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: Modular test pattern to sweep 8 servos using PCA9685.
 *              Prepared for FSM/MCP integration (per-servo routines).
 **********************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- Custom I2C Pins ---
#define SDA_PIN 6
#define SCL_PIN 7

// --- PCA9685 Servo Parameters ---
#define SERVO_MIN 150  // ≈ 0°
#define SERVO_MAX 600  // ≈ 180°
#define NUM_SERVOS 8   // Number of servos to control
#define SERVO_FREQ 50  // Standard servo freq in Hz

// --- PCA9685 Object ---
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// --- Setup ---
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("🔌 Starting Modular Servo Swing (FSM + MCP Ready)");

  if (!initPCA9685(SDA_PIN, SCL_PIN, SERVO_FREQ)) {
    Serial.println("❌ PCA9685 init failed.");
    while (1);  // Halt if init fails
  }

  Serial.println("✅ PCA9685 Ready. Beginning routine...\n");

  Serial.println("🔍 Scanning for I2C devices...");
  Wire.begin(SDA_PIN, SCL_PIN);  // Ensure correct pins used
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("✅ I2C device found at 0x");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("🔍 Scan complete.");
}

// --- Main Loop ---
void loop() {
  runSelectedRoutine();  // 🔁 FSM/MCP entry point
  delay(3000);           // Prevent overlooping during test
}

/* ---------------------------------------------
 * Core Routine Shell (MCP agent entry)
 * --------------------------------------------- */
void runSelectedRoutine() {
  runRoutine_sweep();
}

/* ---------------------------------------------
 * Routine: Swing all servos in sequence
 * --------------------------------------------- */
void runRoutine_sweep() {
  for (int ch = 0; ch < NUM_SERVOS; ch++) {
    Serial.printf("🔁 Swinging Servo %02d...\n", ch);
    swingServo(ch);
    delay(300);
  }

  bonusSwingServo7();  // ✨ Bonus action after sweep
}

/* ---------------------------------------------
 * Modular Action: Swing a single servo back/forth
 * Replace with FSM routines (e.g., openGate, grabObject)
 * --------------------------------------------- */
void swingServo(uint8_t ch) {
  for (int pos = SERVO_MIN; pos <= SERVO_MAX; pos += 10) {
    pwm.setPWM(ch, 0, pos);
    delay(15);
  }

  for (int pos = SERVO_MAX; pos >= SERVO_MIN; pos -= 10) {
    pwm.setPWM(ch, 0, pos);
    delay(15);
  }
}

/* ---------------------------------------------
 * Convert degrees (0–180) to PWM signal
 * and move servo on given channel.
 * --------------------------------------------- */
void moveToAngle(uint8_t ch, int degrees) {
  degrees = constrain(degrees, 0, 180);  // Safety clamp
  int pulse = map(degrees, 0, 180, SERVO_MIN, SERVO_MAX);
  Serial.printf("🎯 Servo %02d → %3d° (PWM: %d)\n", ch, degrees, pulse);
  pwm.setPWM(ch, 0, pulse);
}

/* ---------------------------------------------
 * Modular Init: Set up PCA9685 on given pins/frequency
 * --------------------------------------------- */
bool initPCA9685(uint8_t sdaPin, uint8_t sclPin, uint16_t freq) {
  Wire.begin(sdaPin, sclPin);
  Wire.setClock(400000);  // Fast I2C
  pwm.begin();
  pwm.setPWMFreq(freq);
  delay(100);

  Serial.println("✅ PCA9685 initialized via initPCA9685()");
  return true;
}

/* ---------------------------------------------
 * Bonus Action: Servo 7 swings 120° back & forth x3
 * --------------------------------------------- */
void bonusSwingServo7() {
  const int ch = 7;
  const int minAngle = 30;
  const int maxAngle = 150;

  Serial.println("✨ Bonus: Servo 07 special swing (3x 120°)");

  for (int i = 0; i < 3; i++) {
    moveToAngle(ch, maxAngle);
    delay(500);
    moveToAngle(ch, minAngle);
    delay(500);
  }
}

/* ------------------------------------------------------
 * 🧠 FSM Integration Note:
 * Replace swingServo() with stateful behaviors like:
 *   void moveToAngle(uint8_t ch, int degrees);
 *   void handleFSMGate(uint8_t ch, String state);
 * Or trigger via NAS/SD JSON motion profiles per agent.
 * ------------------------------------------------------ */