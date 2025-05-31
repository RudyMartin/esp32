/**********************************************************************
* Filename    : 2-ServoControlFSM.ino
* Description : Modular FSM example for Lab 2 - controlling a servo motor
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#include <Arduino.h>
#include <Servo.h>

#define SERVO_PIN 5

Servo myServo;

// === FSM Types ===
typedef void (*StateHandler)();
typedef void (*LifecycleHook)();

struct StateConfig {
  const char* name;
  StateHandler handler;
  unsigned long duration;
  const char* next;
  LifecycleHook onEnter;
  LifecycleHook onExit;
};

// === FSM State Vars ===
int currentStateIndex = 0;
unsigned long stateStartTime = 0;
unsigned long lastActionTime = 0;

// === Forward Declarations ===
void moveTo90State();
void moveTo0State();
void servoAttach();
void servoDetach();

StateConfig states[] = {
  { "MOVE_90", moveTo90State, 2000, "MOVE_0", servoAttach, servoDetach },
  { "MOVE_0",  moveTo0State,  2000, "MOVE_90", servoAttach, servoDetach }
};
const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

// === State Functions ===
void moveTo90State() {
  // No-op during state duration
}

void moveTo0State() {
  // No-op during state duration
}

void servoAttach() {
  myServo.attach(SERVO_PIN);
  if (strcmp(states[currentStateIndex].name, "MOVE_90") == 0) {
    Serial.println("[ENTER] MOVE_90: rotating to 90");
    myServo.write(90);
  } else {
    Serial.println("[ENTER] MOVE_0: rotating to 0");
    myServo.write(0);
  }
  lastActionTime = millis();
}

void servoDetach() {
  Serial.println("[EXIT] Servo released");
  myServo.detach();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.print("Starting in state: ");
  Serial.println(states[currentStateIndex].name);
  stateStartTime = millis();
  lastActionTime = millis();
  if (states[currentStateIndex].onEnter) states[currentStateIndex].onEnter();
}

void loop() {
  unsigned long now = millis();
  StateConfig current = states[currentStateIndex];

  current.handler();

  if (current.duration > 0 && now - stateStartTime >= current.duration) {
    if (current.onExit) current.onExit();

    for (int i = 0; i < NUM_STATES; i++) {
      if (strcmp(states[i].name, current.next) == 0) {
        currentStateIndex = i;
        break;
      }
    }
    stateStartTime = millis();
    lastActionTime = millis();
    if (states[currentStateIndex].onEnter) states[currentStateIndex].onEnter();
  }
}
