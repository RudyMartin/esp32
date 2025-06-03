/**********************************************************************
* Filename    : 5-MissionDeckFSM.ino
* Description : Multi-step FSM combining sensor read, servo move, and logging
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#include <Arduino.h>
#include <Servo.h>
#include "sensor_utils.h"
#include "servo_utils.h"
#include "ActionParams.h"

#define SENSOR_PIN A0
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

// === State Forward Declarations ===
void waitForObject();
void moveServoReveal();
void showLEDFeedback();
void logResult();
void servoEnterFromParams();
void servoExitDetach();

// === Sensor Data ===
float lastReading = 0.0;

// === Servo ActionParam ===
ActionParams moveTo90 = { SERVO_PIN, 90, 1000 };

// === FSM Config ===
StateConfig states[] = {
  { "WAIT_FOR_OBJECT", waitForObject, 1000, "MOVE_SERVO", nullptr, nullptr },
  { "MOVE_SERVO", moveServoReveal, 1000, "SHOW_LED", servoEnterFromParams, servoExitDetach },
  { "SHOW_LED", showLEDFeedback, 1000, "LOG", nullptr, nullptr },
  { "LOG", logResult, 1000, "WAIT_FOR_OBJECT", nullptr, nullptr }
};
const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

// === State Handlers ===
void waitForObject() {
  lastReading = readAnalogSensor(SENSOR_PIN);
  Serial.print("[SENSE] Light level: ");
  Serial.println(lastReading);
}

void moveServoReveal() {
  // Servo is moved in onEnter
}

void showLEDFeedback() {
  Serial.println("[LED] Object detected. Status: OK");
}

void logResult() {
  Serial.print("{ \"sensor\": \"A0\", \"value\": ");
  Serial.print(lastReading);
  Serial.println(" } // Mission log entry");
}

void servoEnterFromParams() {
  myServo.attach(moveTo90.pin);
  Serial.print("[SERVO] Rotating to ");
  Serial.println(moveTo90.angle);
  myServo.write(moveTo90.angle);
  delay(moveTo90.duration); // optional for now
}

void servoExitDetach() {
  Serial.println("[SERVO] Detaching");
  myServo.detach();
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  Serial.print("Starting in state: ");
  Serial.println(states[currentStateIndex].name);
  stateStartTime = millis();
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
    if (states[currentStateIndex].onEnter) states[currentStateIndex].onEnter();
  }
}
