/**********************************************************************
* Filename    : 4-ConditionalFSM.ino
* Description : FSM with button-triggered state transition
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#include <Arduino.h>
#include "button_utils.h"

#define BUTTON_PIN 12
#define LED_PIN 2

Button btn;

// === FSM Types ===
typedef void (*StateHandler)();
typedef void (*LifecycleHook)();
typedef bool (*AdvanceCheck)();

struct StateConfig {
  const char* name;
  StateHandler handler;
  unsigned long duration;
  const char* next;
  LifecycleHook onEnter;
  LifecycleHook onExit;
  AdvanceCheck shouldAdvance;
};

// === FSM State Vars ===
int currentStateIndex = 0;
unsigned long stateStartTime = 0;

// === Declarations ===
void idleState();
void blinkState();
bool buttonPressedCheck();

StateConfig states[] = {
  { "IDLE",  idleState, 0, "BLINK", nullptr, nullptr, buttonPressedCheck },
  { "BLINK", blinkState, 3000, "IDLE", nullptr, nullptr, nullptr }
};
const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

// === State Functions ===
void idleState() {
  // Just wait for button
  digitalWrite(LED_PIN, LOW);
  Serial.println("IDLE... waiting for button press");
  delay(200);
}

void blinkState() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("BLINKING!");
}

bool buttonPressedCheck() {
  return isButtonPressed(btn);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  setupButton(btn, BUTTON_PIN);

  Serial.print("Starting in state: ");
  Serial.println(states[currentStateIndex].name);
  stateStartTime = millis();
  if (states[currentStateIndex].onEnter) states[currentStateIndex].onEnter();
}

void loop() {
  unsigned long now = millis();
  StateConfig current = states[currentStateIndex];

  current.handler();

  bool shouldTransition = false;
  if (current.shouldAdvance && current.shouldAdvance()) {
    shouldTransition = true;
  } else if (current.duration > 0 && now - stateStartTime >= current.duration) {
    shouldTransition = true;
  }

  if (shouldTransition) {
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
