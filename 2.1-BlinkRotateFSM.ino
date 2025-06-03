/**********************************************************************
 * File       : 2.1-BlinkRotateFSM.ino
 * Title      : MCP/FSM Example – Blink + Rotate
 * Author     : Rudy Martin / Next Shift Consulting
 * Description: Intro to FSM with LED blink and rotation
 *              Teaches states, millis(), and MCP loop
 **********************************************************************/

// --- Pin Setup ---
const int LED_PIN = 13;  // Built-in LED

// --- FSM States ---
enum State {
  BLINK,
  ROTATE
};
State currentState = BLINK;

// --- Timing Variables ---
unsigned long stateStartTime = 0;
const unsigned long DURATION = 2000;  // 2 seconds per state

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  stateStartTime = millis();
  Serial.println("FSM Initialized");
}

void loop() {
  // MCP Step 1: SENSE (get time)
  unsigned long currentTime = millis();

  // MCP Step 2: PLAN (decide based on time)
  if (currentTime - stateStartTime >= DURATION) {
    // Time to switch state
    currentState = (currentState == BLINK) ? ROTATE : BLINK;
    stateStartTime = currentTime;

    // MCP Step 4: LOG
    Serial.print("Switching to state: ");
    Serial.println((currentState == BLINK) ? "BLINK" : "ROTATE");
  }

  // MCP Step 3: ACT (run behavior based on state)
  switch (currentState) {
    case BLINK:
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
      break;

    case ROTATE:
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
      }
      break;
  }

  // MCP Step 5: REPEAT – loop continues
}
