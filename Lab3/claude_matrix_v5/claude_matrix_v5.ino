/**********************************************************************
 * Filename    : MissionCrew_Lab3_Fixed.ino
 * Description : SEE/THINK/ACT/SPEAK/REPEAT LED Matrix (64x64 HUB75)
 * Author      : Rudy Martin / Next Shift Consulting - FIXED VERSION
 * Project     : Artemis DSAI 2025 – LED Matrix Display Labs Using Agents
 * Modified    : Jun 4, 2025   
 * FIXED: Watchdog task not found errors and display initialization
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

// === Timing - SIMPLIFIED WITHOUT WATCHDOG ===
unsigned long lastFeedTime = 0;

// === SEE ===
String imageDirectory = "/images/";
String current_directive = "default";
unsigned long wait_time = 3000;

// === Simple yield function - NO WATCHDOG ===
void safeYield() {
  unsigned long now = millis();
  if (now - lastFeedTime > 50) {  // Yield every 50ms
    yield();
    delay(1);
    lastFeedTime = now;
  }
}

// === Non-blocking delay function ===
void safeDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    safeYield();
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
  safeYield();
  
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
  safeYield();
}

void dataCleanerAgent(String param) {
  Serial.println("[DataCleaner] Processing: " + param);
  safeYield();
  
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
  safeYield();
  
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
  safeDelay(2000); // Initial delay for serial
  
  Serial.println("\n=================================");
  Serial.println("[INIT] ESP32-S3 HUB75 Display - FIXED VERSION");
  Serial.println("[INIT] Watchdog disabled to prevent task errors");
  Serial.println("=================================");

  // SPIFFS initialization
  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Mount failed - continuing without SPIFFS");
  }
  safeYield();

  // === SIMPLIFIED DISPLAY INITIALIZATION ===
  Serial.println("[INIT] Initializing display with ESP32-S3 optimized settings...");
  
  // Single, tested configuration for ESP32-S3
  HUB75_I2S_CFG::i2s_pins _pins = {
    .r1 = 42, .g1 = 41, .b1 = 40,      // Top half RGB
    .r2 = 39, .g2 = 38, .b2 = 37,      // Bottom half RGB  
    .a = 45, .b = 48, .c = 47, .d = 21, .e = 14,  // Address lines
    .lat = 2, .oe = 1, .clk = 36        // Control lines
  };

// // Configuration 3: Hybrid  Approach ESP32-S3, Freenove focused)
// PinConfig config3[] = {
//   {42, "R1", "Red Upper", "Blue"},     // Pin 1 (Keep working RGB)
//   {41, "G1", "Green Upper", "Green"},  // Pin 2 (Keep working RGB)
//   {40, "B1", "Blue Upper", "Yellow"},  // Pin 3 (Keep working RGB)
//   {39, "R2", "Red Lower", "Orange"},   // Pin 4 (Keep working RGB)
//   {38, "G2", "Green Lower", "Red"},    // Pin 5 (Keep working RGB)
//   {37, "B2", "Blue Lower", "Brown"},   // Pin 6 (Keep working RGB)
//   {45, "A", "Address A", "Black"},     // Pin 7 (Keep, generally fine)
//   {48, "B", "Address B", "White"},     // Pin 8 (Keep, generally fine)
//   {47, "C", "Address C", "Silver"},    // Pin 9 (Keep, generally fine)
//   {21, "D", "Address D", "Purple"},    // Pin 10 (Changed from 21: Safer GPIO)
//   {14, "E", "Address E", "Blue"},      // Pin 11 (Changed from 14: Safer GPIO)
//   {-1, "GND", "Ground", "Green"},      // Pin 12 (Ground)
//   {2, "LAT", "Latch", "Yellow"},      // Pin 13 (Changed from 2: Avoids UART TX)
//   {1, "OE", "Output Enable", "Orange"},// Pin 14 (Changed from 1: Avoids UART RX)
//   {36, "CLK", "Clock", "Red"},         // Pin 15 (Changed from 36: Safer, contiguous)
//   {-1, "5V", "Power 5V", "Brown"}      // Pin 16 (Power)
// };


  HUB75_I2S_CFG mxconfig(64, 64, 1, _pins);
  mxconfig.double_buff = true;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::SHIFTREG;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;  // Slower clock for stability
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  safeYield();
  
  if (dma_display->begin()) {
    Serial.println("[SUCCESS] Display initialized!");
    dma_display->setBrightness8(40);  // Lower brightness for stability
    dma_display->clearScreen();
    
    // Simple success indication
    dma_display->fillScreenRGB888(0, 255, 0);  // Green flash
    delay(1000);
    dma_display->clearScreen();
    
    Serial.println("[INIT] Running basic display test...");
    testDisplay();
    
  } else {
    Serial.println("[ERROR] Display initialization failed!");
    Serial.println("[ERROR] Check wiring and power supply (5V, 2A minimum)");
    Serial.println("[ERROR] Verify pin connections match the pinout above");
    dma_display = nullptr;
  }

  Serial.println("[INIT] Setup complete - starting FSM");
  Serial.println("[INFO] If display doesn't work, check:");
  Serial.println("       - 5V power supply (2A minimum)");
  Serial.println("       - All 16 data/control wire connections");
  Serial.println("       - Ground connections between ESP32 and matrix");
}

void loadMissionDirective() {
  safeYield();
  File file = SPIFFS.open("/mission_directive.json", "r");
  if (!file) {
    Serial.println("[THINK] No mission directive found. Using defaults.");
    return;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, file);
  safeYield();
  
  if (error) {
    Serial.println("[THINK] Failed to parse mission directive JSON.");
    file.close();
    return;
  }

  current_directive = doc["policy"] | "default";
  Serial.println("[THINK] Mission Control policy updated: " + current_directive);
  file.close();
  safeYield();
}

bool isImageFile(String name) {
  return name.endsWith(".h") || name.endsWith(".gif") || name.endsWith(".bmp") || name.endsWith(".jpg") || name.endsWith(".png");
}

void useHeaders() {
  Serial.println("[PIPELINE] Running header-based image pipeline.");
  String filenames[] = {"cat64.h", "fire.h", "ThisIsFine64.h"};
  int fileCount = sizeof(filenames) / sizeof(filenames[0]);

  for (int cycle = 0; cycle < 3; cycle++) {
    safeYield();
    for (int i = 0; i < fileCount; i++) {
      Serial.println("[CYCLE] " + String(cycle) + " Image: " + filenames[i]);
      
      // Direct function calls - so simple!
      processImage(filenames[i]);
      safeDelay(3000);
      safeYield();
      
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
  safeYield();
  
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
      safeYield();
      break;

    case THINK:
      Serial.println("[FSM] THINK - Preparing behavior...");
      currentState = ACT;
      safeYield();
      break;

    case ACT:
      Serial.println("[FSM] ACT - Executing behavior...");
      useHeaders();
      currentState = SPEAK;
      safeYield();
      break;

    case SPEAK:
      Serial.println("[FSM] SPEAK - Logging complete.");
      currentState = REPEAT;
      safeYield();
      break;

    case REPEAT:
      Serial.println("[FSM] REPEAT - Waiting for next cycle...");
      safeDelay(wait_time);
      currentState = SEE;
      safeYield();
      break;
  }
}