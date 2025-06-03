/**********************************************************************
* Filename    : 3-SensorLogFSM.ino
* Description : FSM example for reading and logging sensor data
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#include <Arduino.h>
#include "sensor_utils.h"

// === Pin Definitions ===
#define SENSOR_PIN A0

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

// === FSM Variables ===
int currentStateIndex = 0;
unsigned long stateStartTime = 0;
unsigned long lastActionTime = 0;

// === Forward Declarations ===
void readSensorState();
void logSensorState();

StateConfig states[] = {
  { "READ_SENSOR", readSensorState, 1000, "LOG", nullptr, nullptr },
  { "LOG", logSensorState, 1000, "READ_SENSOR", nullptr, nullptr }
};
const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

// === Sensor Data Storage ===
float latestReading = 0.0;

void readSensorState() {
  latestReading = readAnalogSensor(SENSOR_PIN);
  Serial.print("[READ] Sensor value: ");
  Serial.println(latestReading);
}

void logSensorState() {
  Serial.print("{ \"sensor\": \"A0\", \"value\": ");
  Serial.print(latestReading);
  Serial.println(" } // JSON-style log");
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
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

