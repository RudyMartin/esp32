/**********************************************************************
* Filename    : BlockClassifierFSM.ino
* Description : Lab 2 - FSM-based Image Capture and Classification
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>

// HINT: Uncomment and configure WiFi
// const char* ssid = "your_SSID";
// const char* password = "your_PASSWORD";

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

int currentStateIndex = 0;
unsigned long stateStartTime = 0;

// === Camera Config ===
void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.println("[CAMERA] Initialized");
}

// === State Declarations ===
void captureState();
void classifyState();
void transmitState();
void waitState();

// === State Configurations ===
void enterCapture() {
  Serial.println("[FSM] Capturing image...");
  // Placeholder: real capture happens in state handler
}

void enterClassify() {
  Serial.println("[FSM] Running classification...");
  // Placeholder: real classification happens in state handler
}

void enterTransmit() {
  Serial.println("[FSM] Uploading to NAS...");
  // Placeholder: replace with actual upload code
}

void captureState() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[ERROR] Camera capture failed");
    return;
  }
  Serial.printf("[CAPTURED] Image size: %d bytes\n", fb->len);
  esp_camera_fb_return(fb);
}

void classifyState() {
  // HINT: Replace with ML inference or mock output
  Serial.println("[CLASSIFY] Predicted: blue block, small size");
}

void transmitState() {
  // HINT: Replace with HTTP POST to NAS server or local log
  Serial.println("[UPLOAD] Sending JSON result to local NAS");
}

void waitState() {
  // Do nothing
}

StateConfig states[] = {
  { "CAPTURE",   captureState,   1000, "CLASSIFY", enterCapture, nullptr },
  { "CLASSIFY",  classifyState,  1000, "TRANSMIT", enterClassify, nullptr },
  { "TRANSMIT",  transmitState,  1000, "WAIT",     enterTransmit, nullptr },
  { "WAIT",      waitState,      2000, "CAPTURE",  nullptr, nullptr }
};
const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

void setup() {
  Serial.begin(115200);
  delay(500);
  setupCamera();

  // HINT: Uncomment if WiFi needed
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("\n[WIFI] Connected!");

  Serial.print("[FSM] Starting in state: ");
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
