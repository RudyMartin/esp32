/**********************************************************************
 * File       : 3-SensorLogFSM.ino
 * Title      : MCP/FSM – Sensor Logger
 * Author     : Rudy Martin / Next Shift Consulting
 * Description: FSM that reads sensor values and logs them periodically
 *              Emphasizes sensing and logging steps of MCP
 **********************************************************************/

// --- Sensor Setup ---
const int SENSOR_PIN = A0;    // Analog input
const unsigned long INTERVAL = 1000;  // Log every 1s

// --- FSM States ---
enum State {
  IDLE,
  READ_SENSOR
};
State currentState = IDLE;

// --- Timing ---
unsigned long lastLogTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  Serial.println("Sensor FSM Logger Initialized");
  lastLogTime = millis();
}

void loop() {
  unsigned long now = millis();

  switch (currentState) {
    case IDLE:
      // MCP Step 1: SENSE (Check if time to read sensor)
      if (now - lastLogTime >= INTERVAL) {
        currentState = READ_SENSOR;  // MCP Step 2: PLAN
      }
      break;

    case READ_SENSOR:
      // MCP Step 3: ACT (Read + Print)
      int value = analogRead(SENSOR_PIN);
      Serial.print("Sensor Reading: ");
      Serial.println(value);

      // MCP Step 4: LOG (implicitly via Serial)
      lastLogTime = now;
      currentState = IDLE;  // MCP Step 5: REPEAT (loop back)
      break;
  }
}
