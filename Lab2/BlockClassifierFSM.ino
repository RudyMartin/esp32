/**********************************************************************
* Filename    : BlockClassifierFSM.ino
* Description : Lab 2 - FSM-based Image Capture and Classification
* Author      : Rudy Martin / Next Shift Consulting
* Project     : Artemis DSAI 2025 Camp
* Modified    : 2025-05-31
**********************************************************************/

/*
MCP (Model Context Protocol) Overview:
This program acts like a mini-robot brain following a simple plan.

It cycles through a series of steps to continuously monitor its surroundings,
understand them, and then perform actions based on that understanding:

Step 1: SENSE "SEE"   - Like opening your eyes! The system gathers fresh information from the world.
Step 2: PLAN  "THINK"  - Using smarts (like AI models), the system processes the information to understand it.
Step 3: ACT   "DO" - Based on what it now understands, the system figures out what its next important action should be.
Step 4: LOG   "SPEAK" - The system performs the action it decided on. This might be doing something active, or just pausing to pace itself.
Step 5: REPEAT  "LOOP"  - Go back to SENSE and start the whole cycle again, always learning and acting.
*/

#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include "config.h"

// === STEP 0: Setup the FSM (Finite State Machine) ===
// We use a finite state machine to manage the system's behavior in simple, repeatable steps.

// These typedefs define custom names for functions used in each state.
// - StateHandler: The main function that runs for each state (e.g., capture, classify).
// - LifecycleHook: Optional functions that run when entering or exiting a state (like startup messages).
typedef void (*StateHandler)();
typedef void (*LifecycleHook)();

// This struct defines the structure of each state in the FSM.
// Each state includes:
// - name:        A label for the state (e.g., "SEE", "THINK").
// - handler:     The function to run while in this state.
// - duration:    How long to stay in the state (in milliseconds).
// - next:        The name of the next state to transition to.
// - onEnter:     Optional function to run once when entering the state.
// - onExit:      Optional function to run once when exiting the state.
struct StateConfig {
  const char* name;
  StateHandler handler;
  unsigned long duration;
  const char* next;
  LifecycleHook onEnter;
  LifecycleHook onExit;
};

// FSM Control Variables
int currentStateIndex = 0;          // Keeps track of which state we’re in
unsigned long stateStartTime = 0;   // Records when the current state started

// Output Variables from THINK (classification) Step
String label = "unknown";           // The predicted label (e.g., "red_tall")
float confidence = 0.0;             // How confident the model is (from 0.0 to 1.0)
bool usedFallback = false;          // True if a real ML model wasn’t used (fallback was random)

// === SEE: Setup the camera ===
void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5; config.pin_d1 = 18;
  config.pin_d2 = 19; config.pin_d3 = 21;
  config.pin_d4 = 36; config.pin_d5 = 39;
  config.pin_d6 = 34; config.pin_d7 = 35;
  config.pin_xclk = 0; config.pin_pclk = 22;
  config.pin_vsync = 25; config.pin_href = 23;
  config.pin_sscb_sda = 26; config.pin_sscb_scl = 27;
  config.pin_pwdn = 32; config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("[ERROR] Camera initialization failed.");
    return;
  }
  Serial.println("[CAMERA] Ready.");
}

// === Setup WiFi (Optional) ===
void setupWiFi() {
  if (!config.enable_wifi) return;
  WiFi.begin(config.wifi_ssid, config.wifi_password);
  Serial.print("[WIFI] Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WIFI] Connected!");
  Serial.println(WiFi.localIP());
}

// === Show Config ===
void configValidator() {
  Serial.println("===== CONFIG =====");
  Serial.printf("Source      : %s\n", config.source);
  Serial.printf("Sensor      : %s\n", config.sensor);
  Serial.printf("Model       : %s\n", config.model);
  Serial.printf("Experiment  : %s\n", config.experiment);
  Serial.printf("Main Folder : %s\n", config.nas_main_folder);
  Serial.printf("Group Folder: %s\n", config.nas_group_folder);
  Serial.printf("WiFi Enabled: %s\n", config.enable_wifi ? "true" : "false");
  Serial.println("===================");
}

// === FSM State Declarations ===
void captureState();    // SEE
void classifyState();   // THINK
void transmitState();   // SPEAK
void waitState();       // DO (pause before repeating)

void enterCapture()   { Serial.println("[FSM] SEE: Capturing image..."); }
void enterClassify()  { Serial.println("[FSM] THINK: Running classification..."); }
void enterTransmit()  { Serial.println("[FSM] SPEAK: Sending result..."); }

// === FSM State Logic ===

// === SEE ===
void captureState() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[ERROR] Could not capture image.");
    return;
  }
  Serial.printf("[SEE] Captured %d bytes\n", fb->len);
  esp_camera_fb_return(fb);
}

// === THINK ===
void classifyState() {
  // Simulated ML classification
  String labels[] = {"red_tall", "blue_short", "green_round", "yellow_square", "purple_triangle"};
  int num_classes = sizeof(labels) / sizeof(labels[0]);

  if (/* real model not available */ false) {
    // TODO: Add real TFLite model here
  } else {
    usedFallback = true;
    int predictedIndex = random(0, num_classes);
    label = labels[predictedIndex];
    confidence = random(70, 100) / 100.0;
    Serial.printf("[THINK] (Fallback) %s (%.2f)\n", label.c_str(), confidence);
  }
}

// === SPEAK ===
void transmitState() {
  unsigned long timestamp = millis();
  String model_used = usedFallback ? "default" : config.model;

  String result = "{\"label\": \"" + label + "\"," +
                  " \"confidence\": " + String(confidence, 2) + "," +
                  " \"timestamp\": " + String(timestamp) + "," +
                  " \"source\": \"" + config.source + "\"," +
                  " \"sensor\": \"" + config.sensor + "\"," +
                  " \"model\": \"" + model_used + "\" }";

  Serial.println(result);
}

// === DO (Just wait a little before looping again) ===
void waitState() {
  // Intentional pause before repeating
}

// === FSM State Table ===
StateConfig states[] = {
  { "SEE",       captureState,   1000, "THINK",    enterCapture, nullptr },
  { "THINK",     classifyState,  1000, "SPEAK",    enterClassify, nullptr },
  { "SPEAK",     transmitState,  1000, "DO",       enterTransmit, nullptr },
  { "DO",        waitState,      2000, "SEE",      nullptr, nullptr }
};

const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

// === SETUP ===
void setup() {
  Serial.begin(115200);
  delay(500);

  setupCamera();
  setupWiFi();
  configValidator();

  Serial.print("[FSM] Starting in state: ");
  Serial.println(states[currentStateIndex].name);

  stateStartTime = millis();
  if (states[currentStateIndex].onEnter) {
    states[currentStateIndex].onEnter();
  }
}

// === LOOP: REPEAT ===
void loop() {
  unsigned long now = millis();
  StateConfig current = states[currentStateIndex];

  current.handler(); // Run the function for current state

  // Check if it's time to move to the next state
  if (current.duration > 0 && now - stateStartTime >= current.duration) {
    if (current.onExit) current.onExit();

    // Find and switch to the next state
    for (int i = 0; i < NUM_STATES; i++) {
      if (strcmp(states[i].name, current.next) == 0) {
        currentStateIndex = i;
        break;
      }
    }

    stateStartTime = millis();
    if (states[currentStateIndex].onEnter) {
      states[currentStateIndex].onEnter();
    }
  }
}

