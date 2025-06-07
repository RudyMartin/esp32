/**********************************************************************
 * Filename    : L3_matrix_display_llm_serial_resilient.ino
 * Description : LLM Classifier FSM + Serial JSON + Fallback
 * Author      : Rudy Martin / Next Shift Consulting 
 * Project     : Artemis DSAI 2025 – LED Matrix Display Labs Using Agents
 * Modified    : Jun 6, 2025   
 * PURPOSE     : FSM LED matrix display that loops on serial, falls back on error
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>
#include <ArduinoJson.h>

// =====================
// TIMING + BEHAVIOR CONFIGS
// =====================
#define SYSTEM_BOOT_DELAY     300
#define STATE_TRANSITION_TIME 3000
#define TIME_ON_SCREEN        60000
#define GLOBAL_LOOP_DELAY     50
#define LOOP_THRESHOLD         3

// =====================
// GPIO PinMap Struct
// =====================
struct PinMap {
  int R1 = 42, G1 = 41, B1 = 40;
  int R2 = 38, G2 = 39, B2 = 37;
  int A = 45, B = 48, C = 47, D = 15, E = 16;
  int LAT = 21, OE = 18, CLK = 20;
} pins;

// =====================
// Display + FSM Setup
// =====================
#define PANEL_RES_X 64
#define PANEL_RES_Y 64
#define PANEL_CHAIN 1
MatrixPanel_I2S_DMA *dma_display = nullptr;

enum DisplayState {
  STATE_SENSE_INPUT,      // 1. MCP: Sense — Read Serial or Input
  STATE_DECIDE_ACTION,    // 2. MCP: Plan — Parse JSON and decide what to do
  STATE_ACT_ON_DECISION,  // 3. MCP: Act — Update LED matrix with results
  STATE_LOG_RESULT,       // 4. MCP: Log — Output decision to Serial
  STATE_WAIT_NEXT_INPUT,  // 5. MCP: Repeat — Delay and loop back
  STATE_ERROR             // Error handler
};

DisplayState fsm_loops++;
  currentState = STATE_SENSE_INPUT;
unsigned long stateTimer = 0;

// =====================
// LLM Data Holders
// =====================
String llm_label = "UNKNOWN";
bool llm_accepted = false;
float llm_confidence = 0.5;
bool llm_data_ready = false;
int displayLoops = 0;

// =====================
// Setup
// =====================

// =====================
// Loop Control
// =====================
int fsm_loops = 0;
const int MAX_LOOPS = 10;

void setup() {
  Serial.begin(115200);
  delay(SYSTEM_BOOT_DELAY);
  Serial.println("✅ Boot complete. Awaiting LLM input...");
  fsm_loops++;
  currentState = STATE_SENSE_INPUT;
  stateTimer = millis();
}

// =====================
// Loop
// =====================
void loop() {
  handleSerialLLMInput();

  switch (currentState) {
    case STATE_SENSE_INPUT: handleSenseInputState(); break;
    case STATE_DECIDE_ACTION: handleDecideActionState(); break;
    case STATE_ACT_ON_DECISION: handleActOnDecisionState(); break;
    case STATE_LOG_RESULT: handleLogResultState(); break;
    case STATE_WAIT_NEXT_INPUT: handleWaitNextInputState(); break;
    case STATE_ERROR: handleErrorState(); break;
  }

  
  // End after MAX_LOOPS if reached
  if (fsm_loops >= MAX_LOOPS) {
    Serial.println("[FSM] Max loops reached. Halting.");
    while (true) delay(1000);
  }

  delay(GLOBAL_LOOP_DELAY);
}

// =====================
// FSM State Handlers
// =====================
void handleInitState() {
  Serial.println("[INIT] Setting up matrix...");

  HUB75_I2S_CFG::i2s_pins _pins = {
    pins.R1, pins.G1, pins.B1,
    pins.R2, pins.G2, pins.B2,
    pins.A, pins.B, pins.C, pins.D, pins.E,
    pins.LAT, pins.OE, pins.CLK
  };

  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN, _pins);
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_8M;
  mxconfig.clkphase = true;
  mxconfig.double_buff = true;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  if (!dma_display->begin()) {
    Serial.println("[ERROR] Display init failed.");
    currentState = STATE_ERROR;
    return;
  }

  dma_display->setBrightness8(90);
  dma_display->clearScreen();
  currentState = STATE_DRIVER_SETUP;
  stateTimer = millis();
}

void handleDriverSetupState() {
  if (millis() - stateTimer < STATE_TRANSITION_TIME) return;

  Serial.println("[DRIVER] Display ready.");
  currentState = STATE_DISPLAY_LLM_RESULT;
  stateTimer = millis();
}

void handleLLMResultState() {
  if (!llm_data_ready || millis() - stateTimer < TIME_ON_SCREEN) return;

  showLLMResult(llm_label, llm_accepted, llm_confidence);
  llm_data_ready = false;
  stateTimer = millis();
  displayLoops++;

  if (displayLoops >= LOOP_THRESHOLD) {
    StaticJsonDocument<200> doc;
    doc["label"] = llm_label;
    doc["accepted"] = llm_accepted;
    doc["confidence"] = llm_confidence;
    serializeJson(doc, Serial);
    Serial.println();
  }
}

void handleErrorState() {
  Serial.println("[ERROR] Halting system. Check wiring.");
  while (true) delay(1000);
}

// =====================
// Input Handler with Fallback
// =====================
void handleSerialLLMInput() {
  static String incoming = "";

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '
') {
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, incoming);

      if (!error && doc.containsKey("label")) {
        llm_label = doc["label"].as<String>();
        llm_accepted = doc["accepted"];
        llm_confidence = doc["confidence"];
        Serial.println("✅ Parsed LLM result");
      } else {
        llm_label = "UNKNOWN";
        llm_accepted = false;
        llm_confidence = 0.5;
        Serial.println("⚠️ Invalid JSON. Using fallback values.");
      }

      llm_data_ready = true;
      incoming = "";
    } else {
      incoming += c;
    }
  }
}

// =====================
// Output Display
// =====================
void showLLMResult(String label, bool accepted, float confidence) {
  dma_display->clearScreen();

  dma_display->setTextSize(1);
  dma_display->setTextColor(dma_display->color565(0, 255, 255));
  dma_display->setCursor(4, 4);
  dma_display->print("LABEL:");
  dma_display->setCursor(40, 4);
  dma_display->print(label);

  dma_display->setCursor(4, 20);
  dma_display->setTextColor(accepted ? dma_display->color565(0, 255, 0) : dma_display->color565(255, 0, 0));
  dma_display->print(accepted ? "ACCEPTED" : "REJECTED");

  dma_display->drawRect(4, 35, 56, 6, dma_display->color565(255, 255, 255));
  int barWidth = int(confidence * 56.0);
  dma_display->fillRect(4, 35, barWidth, 6, dma_display->color565(0, 128, 255));

  dma_display->setCursor(4, 46);
  dma_display->setTextColor(dma_display->color565(255, 255, 0));
  dma_display->print("CONF:");
  dma_display->setCursor(40, 46);
  dma_display->print(String(confidence * 100, 1) + "%");
}
