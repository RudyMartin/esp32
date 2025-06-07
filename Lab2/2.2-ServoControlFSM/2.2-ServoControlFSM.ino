/**********************************************************************
 * File       : 2.2-ServoControlFSM.ino
 * Title      : MCP/FSM – Servo Sweep Controller
 * Author     : Rudy Martin / Next Shift Consulting
 * Description: FSM with servo movement logic
 *              Teaches Action FSM + MCP transitions
 **********************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- Servo Config ---
#define SERVO_FREQ 50  // Hz
#define SERVO_MIN 150  // Pulse for 0 deg
#define SERVO_MAX 600  // Pulse for 180 deg
#define SERVO_MID 375  // ≈90°
#define CHANNEL 0      // Using channel 0

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// --- FSM States ---
enum State {
  SWEEP_LEFT,
  SWEEP_RIGHT
};
State currentState = SWEEP_LEFT;

// --- Timing ---
unsigned long stateStartTime = 0;
const unsigned long DURATION = 2000;  // 2 seconds

void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.println("Servo FSM Initialized");
  stateStartTime = millis();
}

void loop() {
  // MCP Step 1: SENSE – get time
  unsigned long now = millis();

  // MCP Step 2: PLAN – time-based state change
  if (now - stateStartTime >= DURATION) {
    currentState = (currentState == SWEEP_LEFT) ? SWEEP_RIGHT : SWEEP_LEFT;
    stateStartTime = now;

    // MCP Step 4: LOG
    Serial.print("Transitioning to: ");
    Serial.println((currentState == SWEEP_LEFT) ? "SWEEP_LEFT" : "SWEEP_RIGHT");
  }

  // MCP Step 3: ACT – execute motion
  switch (currentState) {
    case SWEEP_LEFT:
      pwm.setPWM(CHANNEL, 0, SERVO_MIN);
      break;
    case SWEEP_RIGHT:
      pwm.setPWM(CHANNEL, 0, SERVO_MAX);
      break;
  }

  // MCP Step 5: REPEAT
}
