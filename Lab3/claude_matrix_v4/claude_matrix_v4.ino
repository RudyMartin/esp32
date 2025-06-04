/**********************************************************************
 * Filename    : MissionCrew_Lab3.ino
 * Description : SEE/THINK/ACT/SPEAK/REPEAT LED Matrix (64x64 HUB75)
 * Author      : Rudy Martin / Next Shift Consulting
 * Project     : Artemis DSAI 2025 – LED Matrix Display Labs Using Agents
 * Modified    : Jun 3, 2025   
 * CLEAN VERSION - NO POINTERS! Simple agent system with direct calls
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "esp_task_wdt.h"

// === FSM STATE LOGIC ===
enum SystemState { SEE, THINK, ACT, SPEAK, REPEAT };
SystemState currentState = SEE;

// === Display Setup ===
MatrixPanel_I2S_DMA* dma_display = nullptr;

// === Timing and Watchdog ===
unsigned long lastWatchdogFeed = 0;
const unsigned long WATCHDOG_FEED_INTERVAL = 100;

// === SEE ===
String imageDirectory = "/images/";
String current_directive = "default";
unsigned long wait_time = 3000;

// === Helper function to feed watchdog safely ===
void feedWatchdog() {
  unsigned long now = millis();
  if (now - lastWatchdogFeed > WATCHDOG_FEED_INTERVAL) {
    esp_task_wdt_reset();
    yield();
    lastWatchdogFeed = now;
  }
}

// === Non-blocking delay function ===
void safeDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    feedWatchdog();
    delay(10);
  }
}

// === Test display functionality ===
void testDisplay() {
  if (dma_display == nullptr) {
    Serial.println("[TEST] Display pointer is NULL!");
    return;
  }
  
  Serial.println("[TEST] Starting display test...");
  
  // Test 1: Clear screen
  dma_display->clearScreen();
  Serial.println("[TEST] Screen cleared");
  safeDelay(500);
  
  // Test 2: Fill with red
  dma_display->fillScreenRGB888(255, 0, 0);
  Serial.println("[TEST] Red fill - should see red display");
  safeDelay(2000);
  
  // Test 3: Fill with green  
  dma_display->fillScreenRGB888(0, 255, 0);
  Serial.println("[TEST] Green fill - should see green display");
  safeDelay(2000);
  
  // Test 4: Fill with blue
  dma_display->fillScreenRGB888(0, 0, 255);
  Serial.println("[TEST] Blue fill - should see blue display");
  safeDelay(2000);
  
  // Test 5: Simple pattern
  dma_display->clearScreen();
  for(int x = 0; x < 64; x += 8) {
    for(int y = 0; y < 64; y += 8) {
      dma_display->drawPixel(x, y, dma_display->color565(255, 255, 255));
    }
  }
  Serial.println("[TEST] White dot pattern - should see white dots");
  safeDelay(2000);
  
  // Test 6: Text
  dma_display->clearScreen();
  dma_display->setTextSize(2);
  dma_display->setTextColor(dma_display->color565(255, 255, 0));
  dma_display->setCursor(5, 25);
  dma_display->print("TEST");
  Serial.println("[TEST] Yellow 'TEST' text");
  safeDelay(3000);
}

// === CLEAN AGENT SYSTEM - NO POINTERS! ===

void imageHandlerAgent(String filename) {
  Serial.println("[ImageHandler] Displaying: " + filename);
  feedWatchdog();
  
  if (dma_display == nullptr) {
    Serial.println("[ERROR] Display not initialized!");
    return;
  }
  
  // Create alternating pattern for visual feedback
  dma_display->clearScreen();
  
  // Draw filename
  dma_display->setTextSize(1);
  dma_display->setTextColor(dma_display->color565(255, 255, 255));
  dma_display->setCursor(2, 2);
  dma_display->print("IMG:");
  dma_display->setCursor(2, 12);
  dma_display->print(filename.substring(0, 10));
  
  // Create animated border
  static int borderColor = 0;
  uint16_t colors[] = {
    dma_display->color565(255, 0, 0),    // Red
    dma_display->color565(0, 255, 0),    // Green  
    dma_display->color565(0, 0, 255),    // Blue
    dma_display->color565(255, 255, 0),  // Yellow
    dma_display->color565(255, 0, 255),  // Magenta
    dma_display->color565(0, 255, 255)   // Cyan
  };
  
  dma_display->drawRect(0, 0, 64, 64, colors[borderColor % 6]);
  dma_display->drawRect(2, 2, 60, 60, colors[(borderColor + 1) % 6]);
  borderColor++;
  
  // Add center pattern
  for(int i = 20; i < 44; i += 4) {
    dma_display->drawPixel(i, 32, colors[borderColor % 6]);
    dma_display->drawPixel(32, i, colors[borderColor % 6]);
  }
  
  Serial.println("[ImageHandler] Pattern drawn with colors");
  feedWatchdog();
}

void dataCleanerAgent(String param) {
  Serial.println("[DataCleaner] Processing: " + param);
  feedWatchdog();
  
  if (dma_display != nullptr) {
    dma_display->clearScreen();
    dma_display->setTextSize(1);
    dma_display->setTextColor(dma_display->color565(0, 255, 255)); // Cyan
    dma_display->setCursor(2, 2);
    dma_display->print("CLEAN:");
    dma_display->setCursor(2, 12);
    dma_display->print(param.substring(0, 8));
    
    // Draw cleaning animation
    for(int i = 0; i < 64; i += 4) {
      dma_display->drawPixel(i, 30, dma_display->color565(0, 255, 255));
    }
  }
  
  Serial.println("[DataCleaner] Data cleaning complete");
}

void loggerAgent(String param) {
  Serial.println("[Logger] Logging: " + param);
  feedWatchdog();
  
  if (dma_display != nullptr) {
    dma_display->clearScreen();
    dma_display->setTextSize(1);
    dma_display->setTextColor(dma_display->color565(255, 255, 0)); // Yellow
    dma_display->setCursor(2, 2);
    dma_display->print("LOG:");
    dma_display->setCursor(2, 12);
    dma_display->print(param.substring(0, 9));
    
    // Draw log icon (simple bars)
    for(int y = 20; y < 50; y += 3) {
      dma_display->drawLine(5, y, 35, y, dma_display->color565(255, 255, 0));
    }
  }
  
  Serial.println("[Logger] Logging complete");
}

// CLEAN AGENT DISPATCHER - Simple switch statement, no pointers!
bool runAgent(String agentName, String param) {
  if (agentName == "ImageHandler") {
    imageHandlerAgent(param);
    return true;
  }
  else if (agentName == "DataCleaner") {
    dataCleanerAgent(param);
    return true;
  }
  else if (agentName == "Logger") {
    loggerAgent(param);
    return true;
  }
  else {
    Serial.println("[ERROR] Unknown agent: " + agentName);
    return false;
  }
}

// Even simpler - direct function calls by type
void processImage(String filename) {
  imageHandlerAgent(filename);
}

void cleanData(String data) {
  dataCleanerAgent(data);
}

void logEvent(String event) {
  loggerAgent(event);
}

// === INIT ===
void setup() {
  Serial.begin(115200);
  safeDelay(3000); // Longer delay for serial
  
  Serial.println("\n=================================");
  Serial.println("[INIT] ESP32-S3 HUB75 Display Debug");
  Serial.println("=================================");

  // Configure watchdog - Fixed for newer ESP32 core versions
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 30000,  // 30 seconds timeout
    .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
    .trigger_panic = true
  };
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);
  feedWatchdog();

  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Mount failed - continuing without SPIFFS");
  }
  feedWatchdog();

  // Try multiple pin configurations
  Serial.println("[INIT] Trying pin configuration 1 (ESP32-S3 default)...");
  
  // Configuration 1: ESP32-S3 pins
  HUB75_I2S_CFG::i2s_pins _pins1 = {
    .r1 = 42, .g1 = 41, .b1 = 40,
    .r2 = 38, .g2 = 39, .b2 = 37,
    .a = 45, .b = 48, .c = 47, .d = 21, .e = 14,
    .lat = 2, .oe = 1, .clk = 36
  };

  HUB75_I2S_CFG mxconfig1(64, 64, 1, _pins1);
  mxconfig1.double_buff = true;
  mxconfig1.clkphase = false;
  mxconfig1.driver = HUB75_I2S_CFG::SHIFTREG; // Try SHIFTREG driver first
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig1);
  feedWatchdog();
  
  if (dma_display->begin()) {
    Serial.println("[SUCCESS] Display initialized with config 1!");
    dma_display->setBrightness8(50);
    testDisplay();
  } else {
    Serial.println("[FAILED] Config 1 failed, trying config 2...");
    delete dma_display;
    
    // Configuration 2: Alternative ESP32-S3 pins
    HUB75_I2S_CFG::i2s_pins _pins2 = {
      .r1 = 25, .g1 = 26, .b1 = 27,
      .r2 = 14, .g2 = 12, .b2 = 13,
      .a = 23, .b = 19, .c = 5, .d = 17, .e = 18,
      .lat = 4, .oe = 15, .clk = 16
    };

    HUB75_I2S_CFG mxconfig2(64, 64, 1, _pins2);
    mxconfig2.double_buff = true;
    mxconfig2.driver = HUB75_I2S_CFG::FM6126A; // Try FM6126A driver
    
    dma_display = new MatrixPanel_I2S_DMA(mxconfig2);
    feedWatchdog();
    
    if (dma_display->begin()) {
      Serial.println("[SUCCESS] Display initialized with config 2!");
      dma_display->setBrightness8(50);
      testDisplay();
    } else {
      Serial.println("[FAILED] Config 2 failed, trying config 3...");
      delete dma_display;
      
      // Configuration 3: Your original pins with different driver
      HUB75_I2S_CFG::i2s_pins _pins3 = {
        .r1 = 25, .g1 = 26, .b1 = 27,
        .r2 = 14, .g2 = 12, .b2 = 13,
        .a = 23, .b = 22, .c = 5, .d = 17, .e = 32,
        .lat = 4, .oe = 15, .clk = 16
      };

      HUB75_I2S_CFG mxconfig3(64, 64, 1, _pins3);
      mxconfig3.double_buff = false; // Try without double buffering
      mxconfig3.driver = HUB75_I2S_CFG::SHIFTREG;
      
      dma_display = new MatrixPanel_I2S_DMA(mxconfig3);
      feedWatchdog();
      
      if (dma_display->begin()) {
        Serial.println("[SUCCESS] Display initialized with config 3!");
        dma_display->setBrightness8(50);
        testDisplay();
      } else {
        Serial.println("[CRITICAL] All configurations failed!");
        Serial.println("Check your wiring and power supply!");
        dma_display = nullptr;
      }
    }
  }

  Serial.println("[INIT] Setup complete - starting FSM");
}

void loadMissionDirective() {
  feedWatchdog();
  File file = SPIFFS.open("/mission_directive.json", "r");
  if (!file) {
    Serial.println("[THINK] No mission directive found. Using defaults.");
    return;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, file);
  feedWatchdog();
  
  if (error) {
    Serial.println("[THINK] Failed to parse mission directive JSON.");
    file.close();
    return;
  }

  current_directive = doc["policy"] | "default";
  Serial.println("[THINK] Mission Control policy updated: " + current_directive);
  file.close();
  feedWatchdog();
}

bool isImageFile(String name) {
  return name.endsWith(".h") || name.endsWith(".gif") || name.endsWith(".bmp") || name.endsWith(".jpg") || name.endsWith(".png");
}

void useHeaders() {
  Serial.println("[PIPELINE] Running header-based image pipeline.");
  String filenames[] = {"cat64.h", "fire.h", "ThisIsFine64.h"};
  int fileCount = sizeof(filenames) / sizeof(filenames[0]);

  for (int cycle = 0; cycle < 3; cycle++) {
    feedWatchdog();
    for (int i = 0; i < fileCount; i++) {
      Serial.println("[CYCLE] " + String(cycle) + " Image: " + filenames[i]);
      
      // Direct function calls - so simple!
      processImage(filenames[i]);
      safeDelay(3000);
      feedWatchdog();
      
      // Test other agents
      if (cycle == 1) {
        cleanData(filenames[i]);
        safeDelay(2000);
      }
      if (cycle == 2) {
        logEvent(filenames[i]);
        safeDelay(2000);
      }
    }
  }
}

void loop() {
  feedWatchdog();
  
  if (dma_display == nullptr) {
    Serial.println("[ERROR] Display not working - check setup output!");
    safeDelay(5000);
    return;
  }
  
  switch (currentState) {
    case SEE:
      Serial.println("[FSM] SEE - Loading directive...");
      loadMissionDirective();
      currentState = THINK;
      feedWatchdog();
      break;

    case THINK:
      Serial.println("[FSM] THINK - Preparing behavior...");
      currentState = ACT;
      feedWatchdog();
      break;

    case ACT:
      Serial.println("[FSM] ACT - Executing behavior...");
      useHeaders();
      currentState = SPEAK;
      feedWatchdog();
      break;

    case SPEAK:
      Serial.println("[FSM] SPEAK - Logging complete.");
      currentState = REPEAT;
      feedWatchdog();
      break;

    case REPEAT:
      Serial.println("[FSM] REPEAT - Waiting for next cycle...");
      safeDelay(wait_time);
      currentState = SEE;
      feedWatchdog();
      break;
  }
}