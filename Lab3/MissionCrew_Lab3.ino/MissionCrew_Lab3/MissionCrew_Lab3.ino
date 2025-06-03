/**********************************************************************
 * Filename    : MissionCrew_Lab3.ino
 * Description : SEE/THINK/ACT/SPEAK/REPEAT LED Matrix (64x64 HUB75)
 * Author      : Rudy Martin / Next Shift Consulting
 * Project     : Artemis DSAI 2025 – LED Matrix Display Labs Using Agents
 * Modified    : Jun 3, 2025   
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// === FSM STATE LOGIC ===
enum SystemState { SEE, THINK, ACT, SPEAK, REPEAT };
SystemState currentState = SEE;

// === Display Setup ===
MatrixPanel_I2S_DMA* dma_display = nullptr;
MatrixPanel_Config config;

// === SEE ===
String imageDirectory = "/images/";
String current_directive = "default";
unsigned long wait_time = 3000;

// === AGENT REGISTRY ===
typedef void (*AgentFunc)(String param);

struct Agent {
  String name;
  AgentFunc run;
};

void displayImageAgent(String filename) {
  Serial.println("[ImageHandler] Would display: " + filename);
  // TODO: Implement actual image drawing
}
void dummyAgent(String param) {
  Serial.println("[AGENT] No-op");
}

Agent agentRegistry[] = {
  {"ImageHandler", displayImageAgent},
  {"DataCleaner", dummyAgent},
  {"Logger", dummyAgent}
};

Agent* getAgent(String name) {
  for (int i = 0; i < sizeof(agentRegistry) / sizeof(agentRegistry[0]); i++) {
    if (agentRegistry[i].name == name) return &agentRegistry[i];
  }
  return nullptr;
}

// === INIT ===
void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Mount failed.");
    return;
  }

  // Configure display
  config.mx_width = 64;
  config.mx_height = 64;
  config.chain_length = 1;

  config.gpio.r1 = 25;
  config.gpio.g1 = 26;
  config.gpio.b1 = 27;
  config.gpio.r2 = 14;
  config.gpio.g2 = 12;
  config.gpio.b2 = 13;
  config.gpio.a = 23;
  config.gpio.b = 22;
  config.gpio.c = 5;
  config.gpio.d = 17;
  config.gpio.e = 32;
  config.gpio.lat = 4;
  config.gpio.oe = 15;
  config.gpio.clk = 16;

  dma_display = new MatrixPanel_I2S_DMA(config);
  dma_display->begin();
  dma_display->setBrightness8(64);

  Serial.println("[INIT] System initialized.");
}

void loadMissionDirective() {
  File file = SPIFFS.open("/mission_directive.json", "r");
  if (!file) {
    Serial.println("[THINK] No mission directive found. Using defaults.");
    return;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("[THINK] Failed to parse mission directive JSON.");
    return;
  }

  current_directive = doc["policy"] | "default";
  Serial.println("[THINK] Mission Control policy updated: " + current_directive);
  file.close();
}

void process_image(String filename) {
  Agent* imgAgent = getAgent("ImageHandler");
  if (imgAgent) imgAgent->run(filename);
}

bool isImageFile(String name) {
  return name.endsWith(".h") || name.endsWith(".gif") || name.endsWith(".bmp");
}

bool useSpiffs() {
  Serial.println("[PIPELINE] Trying SPIFFS image pipeline...");
  File root = SPIFFS.open(imageDirectory);
  if (!root || !root.isDirectory()) {
    Serial.println("[SPIFFS] Failed to open image directory.");
    return false;
  }

  File file = root.openNextFile();
  int fileCount = 0;

  while (file && fileCount < 10) {
    if (!isImageFile(file.name())) {
      file = root.openNextFile();
      continue;
    }

    String name = file.name();
    Serial.println("[SPIFFS] Displaying: " + name);
    process_image(name);
    delay(20000);

    file = root.openNextFile();
    fileCount++;
  }

  return fileCount > 0;
}

void useHeaders() {
  Serial.println("[PIPELINE] Defaulting to header-based image pipeline.");
  String filenames[] = {"cat64.h", "fire.h", "ThisIsFine64.h"};
  int fileCount = sizeof(filenames) / sizeof(filenames[0]);

  for (int cycle = 0; cycle < 10; cycle++) {
    for (int i = 0; i < fileCount; i++) {
      process_image(filenames[i]);
      delay(20000);
    }
  }
}

void limitProtocol() {
  if (current_directive == "default") {
    Serial.println("[LIMIT] Using default behavior.");
    useHeaders();
  } else if (current_directive == "explore") {
    Serial.println("[LIMIT] Entering exploratory scan mode.");
    useSpiffs();
  } else if (current_directive == "alert") {
    Serial.println("[LIMIT] Priority override: ALERT loop");
    process_image("warning_banner.h");
    delay(20000);
  } else {
    Serial.println("[LIMIT] Unknown directive. Defaulting to safe mode.");
    useHeaders();
  }
}

void loop() {
  switch (currentState) {
    case SEE:
      Serial.println("[FSM] SEE - Loading directive...");
      loadMissionDirective();
      currentState = THINK;
      break;

    case THINK:
      Serial.println("[FSM] THINK - Preparing behavior...");
      currentState = ACT;
      break;

    case ACT:
      Serial.println("[FSM] ACT - Executing behavior...");
      limitProtocol();
      currentState = SPEAK;
      break;

    case SPEAK:
      Serial.println("[FSM] SPEAK - Logging complete.");
      currentState = REPEAT;
      break;

    case REPEAT:
      Serial.println("[FSM] REPEAT - Waiting for next cycle...");
      delay(wait_time);
      currentState = SEE;
      break;
  }
}

