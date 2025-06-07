/**********************************************************************
 * File       : 5-MissionDeckFSM.ino
 * Title      : MCP/FSM – Full Mission Logic
 * Author     : Rudy Martin / Next Shift Consulting
 * Description: Pipeline FSM: Sense → Act → Log
 *              Models a full robotics mission: detection → action → feedback
 **********************************************************************/

// --- Pin + Servo Setup ---
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
const int SENSOR_PIN = A0;       // Simulated detection input
const int LED_PIN = 13;          // Status indicator
#define SERVO_CHANNEL 0
#define SERVO_MIN 150
#define SERVO_MAX 600
#define SERVO_FREQ 50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// --- FSM States ---
enum State {
  SCAN,
  REACT,
  REPORT
};
State currentState = SCAN;

// --- Timing ---
unsigned long lastChange = 0;
const unsigned long REACT_TIME = 1000;
const unsigned long REPORT_TIME = 1500;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  Serial.println("MissionDeck FSM Initialized");
}

void loop() {
  unsigned long now = millis();

  switch (currentState) {
    case SCAN:
      // MCP Step 1: SENSE – Read simulated sensor
      int val = analogRead(SENSOR_PIN);
      Serial.print("Scan Value: ");
      Serial.println(val);

      // MCP Step 2: PLAN – trigger if threshold crossed
      if (val > 500) {
        currentState = REACT;
        lastChange = now;
        Serial.println("Trigger Detected → REACT");  // MCP Step 4: LOG
      }
      break;

    case REACT:
      // MCP Step 3: ACT – Move servo to simulate sorting
      pwm.setPWM(SERVO_CHANNEL, 0, SERVO_MAX);
      digitalWrite(LED_PIN, HIGH);

      if (now - lastChange >= REACT_TIME) {
        currentState = REPORT;
        lastChange = now;
        Serial.println("Action Complete → REPORT");  // MCP Step 4: LOG
      }
      break;

    case REPORT:
      // MCP Step 4: LOG – Feedback and reset
      pwm.setPWM(SERVO_CHANNEL, 0, SERVO_MIN);
      digitalWrite(LED_PIN, LOW);

      if (now - lastChange >= REPORT_TIME) {
        currentState = SCAN;
        Serial.println("Cycle Complete → SCAN");  // MCP Step 5: REPEAT
      }
      break;
  }
}
