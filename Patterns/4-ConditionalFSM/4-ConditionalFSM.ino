/**********************************************************************
 * File       : 4-ConditionalFSM.ino
 * Title      : MCP/FSM – Button-Triggered State Machine
 * Author     : Rudy Martin / Next Shift Consulting
 * Description: Demonstrates an event-driven FSM
 *              Uses button to trigger conditional transitions
 **********************************************************************/

// --- Pin Setup ---
const int BUTTON_PIN = 2;  // Button input
const int LED_PIN = 13;    // LED output

// --- FSM States ---
enum State {
  WAIT_FOR_PRESS,
  LED_ON
};
State currentState = WAIT_FOR_PRESS;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Conditional FSM Ready");
}

void loop() {
  // MCP Step 1: SENSE – read button
  bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;

  // MCP Step 2: PLAN – event-based decision
  switch (currentState) {
    case WAIT_FOR_PRESS:
      if (buttonPressed) {
        currentState = LED_ON;
        Serial.println("Button Pressed → LED_ON State");  // MCP Step 4: LOG
      }
      break;

    case LED_ON:
      if (!buttonPressed) {
        currentState = WAIT_FOR_PRESS;
        Serial.println("Button Released → WAIT_FOR_PRESS State");  // MCP Step 4: LOG
      }
      break;
  }

  // MCP Step 3: ACT – state behavior
  switch (currentState) {
    case WAIT_FOR_PRESS:
      digitalWrite(LED_PIN, LOW);
      break;
    case LED_ON:
      digitalWrite(LED_PIN, HIGH);
      break;
  }

  // MCP Step 5: REPEAT – loop continues
}
