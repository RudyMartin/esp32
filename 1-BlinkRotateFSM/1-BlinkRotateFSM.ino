/**********************************************************************
* Filename    : 1-BlinkRotateFSM.ino
* Description : Modular FSM example for Lab 1 - blinking and rotating LEDs
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#include <Arduino.h>

// === Pin Setup ===
#define BLINK_LED_PIN 2
const int rotatePins[] = {13, 14, 15};

// === State Management ===
typedef void (*StateHandler)();
typedef void (*LifecycleHook)();

struct StateConfig {
  const char* name;
  StateHandler handler;
  unsigned long duration;
  const char* next;
  LifecycleHook onEnter;
  LifecycleHook onExit;
  const int* controlledPins;
  int numPins;
};

// === State Variables ===
int currentStateIndex = 0;
unsigned long stateStartTime = 0;
unsigned long lastActionTime = 0;
bool ledOn = false;
int rotateIndex = 0;

// === Forward Declarations ===
void blinkState();
void rotateState();
void blinkEnter();
void rotateEnter();
void blinkExit();
void rotateExit();

const int blinkPins[] = {BLINK_LED_PIN};
const int rotatePinsUsed[] = {13, 14, 15};

StateConfig states[] = {
  { "BLINK", blinkState, 10000, "ROTATE", blinkEnter, blinkExit, blinkPins, 1 },
  { "ROTATE", rotateState, 10000, "BLINK", rotateEnter, rotateExit, rotatePinsUsed, 3 }
};
const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

// === State Functions ===
void blinkState() {
  if (millis() - lastActionTime >= 1000) {
    ledOn = !ledOn;
    digitalWrite(BLINK_LED_PIN, ledOn ? HIGH : LOW);
    Serial.println(ledOn ? "LED ON" : "LED OFF");
    lastActionTime = millis();
  }
}

void rotateState() {
  if (millis() - lastActionTime >= 1000) {
    for (int i = 0; i < 3; i++) digitalWrite(rotatePinsUsed[i], LOW);
    digitalWrite(rotatePinsUsed[rotateIndex], HIGH);
    Serial.print("Pin ");
    Serial.print(rotatePinsUsed[rotateIndex]);
    Serial.println(" ON");
    rotateIndex = (rotateIndex + 1) % 3;
    lastActionTime = millis();
  }
}

// === Lifecycle Hooks ===
void blinkEnter() {
  Serial.println("[ENTER] BLINK state");
  ledOn = false;
  digitalWrite(BLINK_LED_PIN, LOW);
}

void blinkExit() {
  Serial.println("[EXIT] BLINK state");
  digitalWrite(BLINK_LED_PIN, LOW);
}

void rotateEnter() {
  Serial.println("[ENTER] ROTATE state");
  rotateIndex = 0;
  for (int i = 0; i < 3; i++) digitalWrite(rotatePinsUsed[i], LOW);
}

void rotateExit() {
  Serial.println("[EXIT] ROTATE state");
  for (int i = 0; i < 3; i++) digitalWrite(rotatePinsUsed[i], LOW);
}

// === Core Arduino Setup/Loop ===
void setup() {
  Serial.begin(115200);
  pinMode(BLINK_LED_PIN, OUTPUT);
  for (int i = 0; i < 3; i++) pinMode(rotatePinsUsed[i], OUTPUT);
  Serial.print("Starting in state: ");
  Serial.println(states[currentStateIndex].name);
  stateStartTime = millis();
  lastActionTime = millis();
  if (states[currentStateIndex].onEnter) states[currentStateIndex].onEnter();
}

void loop() {
  unsigned long now = millis();
  StateConfig current = states[currentStateIndex];

  // Run state behavior
  current.handler();

  // Time-based transition check
  if (current.duration > 0 && now - stateStartTime >= current.duration) {
    if (current.onExit) current.onExit();

    // Transition to next state
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