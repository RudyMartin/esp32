/**********************************************************************
* Filename    : BlockClassifierFSM.ino
* Description : Lab 2 - FSM-based Image Capture and Classification
* Author      : Next Shift Consulting LLC / Artemis DSAI 2025 Camp
* Modified    : 2025-05-31
**********************************************************************/

#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include "config.h"

// === FSM Type Declarations ===
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

// === Global Variables ===
int currentStateIndex = 0;
unsigned long stateStartTime = 0;
String label = "unknown";
float confidence = 0.0;
bool usedFallback = false;

// === Camera Config ===
void setupCamera() {
  camera_config_t config_cam;
  config_cam.ledc_channel = LEDC_CHANNEL_0;
  config_cam.ledc_timer = LEDC_TIMER_0;
  config_cam.pin_d0 = 5;
  config_cam.pin_d1 = 18;
  config_cam.pin_d2 = 19;
  config_cam.pin_d3 = 21;
  config_cam.pin_d4 = 36;
  config_cam.pin_d5 = 39;
  config_cam.pin_d6 = 34;
  config_cam.pin_d7 = 35;
  config_cam.pin_xclk = 0;
  config_cam.pin_pclk = 22;
  config_cam.pin_vsync = 25;
  config_cam.pin_href = 23;
  config_cam.pin_sscb_sda = 26;
  config_cam.pin_sscb_scl = 27;
  config_cam.pin_pwdn = 32;
  config_cam.pin_reset = -1;
  config_cam.xclk_freq_hz = 20000000;
  config_cam.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config_cam.frame_size = FRAMESIZE_QVGA;
    config_cam.jpeg_quality = 10;
    config_cam.fb_count = 2;
  } else {
    config_cam.frame_size = FRAMESIZE_QQVGA;
    config_cam.jpeg_quality = 12;
    config_cam.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config_cam);
  if (err != ESP_OK) {
    Serial.printf("❌ Camera init failed with error 0x%x\n", err);
  } else {
    Serial.println("✅ [CAMERA] Initialized");
  }
}

// === Debug Helper ===
void config_validator() {
  Serial.println("🔧 Configuration Settings:");
  Serial.printf("  Group:      %s\n", config.source);
  Serial.printf("  Sensor:     %s\n", config.sensor);
  Serial.printf("  Model:      %s\n", config.model);
  Serial.printf("  Experiment: %s\n", config.experiment);
  Serial.printf("  NAS Target: %s\n", config.upload_target);
  Serial.printf("  NAS Folder: %s\n", config.nas_group_folder);
  Serial.printf("  WiFi SSID:  %s\n", config.wifi_ssid);
}

// === FSM State Functions ===
void captureState();
void classifyState();
void transmitState();
void waitState();

// === FSM Lifecycle Hooks ===
void enterCapture() {
  Serial.println("[FSM] Entering CAPTURE state");
}

void enterClassify() {
  Serial.println("[FSM] Entering CLASSIFY state");
}

void enterTransmit() {
  Serial.println("[FSM] Entering TRANSMIT state");
}

// === FSM Handlers ===
void captureState() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ [CAPTURE] Failed to grab image");
    return;
  }
  Serial.printf("📸 [CAPTURED] %d bytes\n", fb->len);
  esp_camera_fb_return(fb);
}

void classifyState() {
  String labels[] = {"red_tall", "blue_short", "green_round", "yellow_square", "purple_triangle"};
  int num_classes = sizeof(labels) / sizeof(labels[0]);

  usedFallback = true;  // Replace with actual inference check later
  int predictedIndex = random(0, num_classes);
  label = labels[predictedIndex];
  confidence = random(70, 100) / 100.0;

  Serial.printf("🧠 [CLASSIFY] %s (%.2f) [%s]\n",
                label.c_str(), confidence,
                usedFallback ? "fallback" : "model");
}

void transmitState() {
  unsigned long timestamp = millis();
  String model_used = usedFallback ? "default" : config.model;

  String result = "{\"label\": \"" + label + "\"," +
                  " \"confidence\": " + String(confidence, 2) + "," +
                  " \"timestamp\": " + String(timestamp) + "," +
                  " \"source\": \"" + config.source + "\"," +
                  " \"sensor\": \"" + config.sensor + "\"," +
                  " \"model\": \"" + model_used + "\" }";

  Serial.println("📤 [UPLOAD] " + result);
}

void waitState() {
  // Passive delay for loop timing
}

// === FSM Config ===
StateConfig states[] = {
  { "CAPTURE",   captureState,   1000, "CLASSIFY", enterCapture, nullptr },
  { "CLASSIFY",  classifyState,  1000, "TRANSMIT", enterClassify, nullptr },
  { "TRANSMIT",  transmitState,  1000, "WAIT",     enterTransmit, nullptr },
  { "WAIT",      waitState,      2000, "CAPTURE",  nullptr, nullptr }
};
const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

// === Setup ===
void setup() {
  Serial.begin(115200);
  delay(500);

  setupCamera();
  config_validator();

  if (config.enable_wifi) {
    Serial.print("🌐 Connecting to WiFi: ");
    Serial.println(config.wifi_ssid);
    WiFi.begin(config.wifi_ssid, config.wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\n✅ [WIFI] Connected!");
  }

  Serial.print("🚀 [FSM] Starting at: ");
  Serial.println(states[currentStateIndex].name);
  stateStartTime = millis();
  if (states[currentStateIndex].onEnter) states[currentStateIndex].onEnter();
}

// === Main Loop ===
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
