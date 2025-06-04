/**********************************************************************
 * Filename    : AutoDiagnostic_LEDMatrix.ino
 * Description : FULL AUTO DIAGNOSTIC - Runs continuously, prints everything
 * Author      : Rudy Martin / Next Shift Consulting - AUTO DIAGNOSTIC
 * Project     : Artemis DSAI 2025 – LED Matrix Display Labs Using Agents
 * Modified    : Jun 4, 2025   
 * PURPOSE: AUTOMATIC continuous diagnostics with detailed serial output
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// === AUTO DIAGNOSTIC SYSTEM ===
MatrixPanel_I2S_DMA* dma_display = nullptr;
bool displayFound = false;
int currentTest = 0;
unsigned long testStartTime = 0;
const int TEST_DURATION = 3000;  // 3 seconds per test

// === PIN CONFIGURATIONS TO TEST ===
struct PinConfig {
  String name;
  HUB75_I2S_CFG::i2s_pins pins;
};

PinConfig configs[] = {
  {
    "ESP32-S3 Standard",
    {.r1 = 42, .g1 = 41, .b1 = 40, .r2 = 38, .g2 = 39, .b2 = 37,
     .a = 45, .b = 48, .c = 47, .d = 21, .e = 14, .lat = 2, .oe = 1, .clk = 36}
  },
  {
    "ESP32-S3 Alternative",
    {.r1 = 25, .g1 = 26, .b1 = 27, .r2 = 14, .g2 = 12, .b2 = 13,
     .a = 23, .b = 19, .c = 5, .d = 17, .e = 18, .lat = 4, .oe = 15, .clk = 16}
  },
  {
    "ESP32 Classic",
    {.r1 = 25, .g1 = 26, .b1 = 27, .r2 = 14, .g2 = 12, .b2 = 13,
     .a = 23, .b = 22, .c = 5, .d = 17, .e = 32, .lat = 4, .oe = 15, .clk = 16}
  }
};

const int NUM_CONFIGS = sizeof(configs) / sizeof(configs[0]);

// === DIAGNOSTIC TESTS ===
struct DiagnosticTest {
  String name;
  String description;
  void (*testFunction)();
};

void testFullRed();
void testFullGreen();
void testFullBlue();
void testFullWhite();
void testCornerPixels();
void testHorizontalLines();
void testVerticalLines();
void testBrightnessLevels();
void testColorGradient();
void testTextDisplay();

DiagnosticTest tests[] = {
  {"FULL RED", "Entire screen should be BRIGHT RED", testFullRed},
  {"FULL GREEN", "Entire screen should be BRIGHT GREEN", testFullGreen},
  {"FULL BLUE", "Entire screen should be BRIGHT BLUE", testFullBlue},
  {"FULL WHITE", "Entire screen should be BRIGHT WHITE", testFullWhite},
  {"CORNER PIXELS", "4 colored pixels in corners: Red(TL), Green(TR), Blue(BL), Yellow(BR)", testCornerPixels},
  {"HORIZONTAL LINES", "White horizontal lines across display", testHorizontalLines},
  {"VERTICAL LINES", "White vertical lines across display", testVerticalLines},
  {"BRIGHTNESS TEST", "White screen cycling through brightness levels", testBrightnessLevels},
  {"COLOR GRADIENT", "Red to Green to Blue gradient pattern", testColorGradient},
  {"TEXT DISPLAY", "Yellow 'TEST OK' text in center", testTextDisplay}
};

const int NUM_TESTS = sizeof(tests) / sizeof(tests[0]);

// === UTILITY FUNCTIONS ===
void printSeparator() {
  Serial.println("================================================================");
}

void printHeader(String title) {
  printSeparator();
  Serial.println("🚀 " + title);
  printSeparator();
}

void printStatus(String status, bool success = true) {
  Serial.println((success ? "✅ " : "❌ ") + status);
}

void printInfo(String info) {
  Serial.println("📋 " + info);
}

void printWarning(String warning) {
  Serial.println("⚠️  " + warning);
}

void printError(String error) {
  Serial.println("🔥 ERROR: " + error);
}

void printTestInfo(String testName, String description) {
  Serial.println("\n🧪 TEST: " + testName);
  Serial.println("👀 LOOK FOR: " + description);
  Serial.println("⏱️  Duration: " + String(TEST_DURATION/1000) + " seconds");
}

// === DISPLAY INITIALIZATION ===
bool initializeDisplayWithConfig(int configIndex) {
  if (dma_display != nullptr) {
    delete dma_display;
    dma_display = nullptr;
  }
  
  printInfo("Trying configuration: " + configs[configIndex].name);
  
  // CHANGE THESE VALUES FOR YOUR MATRIX SIZE:
  // 32x32: HUB75_I2S_CFG mxconfig(32, 32, 1, configs[configIndex].pins);
  // 64x32: HUB75_I2S_CFG mxconfig(64, 32, 1, configs[configIndex].pins);
  // 64x64: HUB75_I2S_CFG mxconfig(64, 64, 1, configs[configIndex].pins);
  // 128x32: HUB75_I2S_CFG mxconfig(128, 32, 1, configs[configIndex].pins);
  
  HUB75_I2S_CFG mxconfig(64, 64, 1, configs[configIndex].pins);  // CURRENT: 64x64
  mxconfig.double_buff = true;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::SHIFTREG;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  
  if (dma_display->begin()) {
    dma_display->setBrightness8(255);  // Max brightness for testing
    dma_display->clearScreen();
    
    printStatus("SUCCESS: " + configs[configIndex].name + " configuration works!");
    
    // Print working pin configuration
    Serial.println("\n📌 WORKING PIN CONFIGURATION:");
    auto pins = configs[configIndex].pins;
    Serial.println("   R1=" + String(pins.r1) + " G1=" + String(pins.g1) + " B1=" + String(pins.b1));
    Serial.println("   R2=" + String(pins.r2) + " G2=" + String(pins.g2) + " B2=" + String(pins.b2));
    Serial.println("   A=" + String(pins.a) + " B=" + String(pins.b) + " C=" + String(pins.c) + " D=" + String(pins.d) + " E=" + String(pins.e));
    Serial.println("   LAT=" + String(pins.lat) + " OE=" + String(pins.oe) + " CLK=" + String(pins.clk));
    
    return true;
  } else {
    printStatus("FAILED: " + configs[configIndex].name, false);
    delete dma_display;
    dma_display = nullptr;
    return false;
  }
}

void findWorkingConfiguration() {
  printHeader("AUTOMATIC PIN CONFIGURATION DETECTION");
  
  for (int i = 0; i < NUM_CONFIGS; i++) {
    if (initializeDisplayWithConfig(i)) {
      displayFound = true;
      return;
    }
    delay(1000);
  }
  
  printError("NO WORKING CONFIGURATION FOUND!");
  printWarning("This indicates a hardware problem:");
  Serial.println("   🔌 Check 5V power supply (2-4 Amps minimum)");
  Serial.println("   🔗 Verify all 16 data wire connections");
  Serial.println("   ⚡ Ensure ground connection between ESP32 and matrix");
  Serial.println("   🧪 Test LED matrix with known-good controller");
}

// === TEST FUNCTIONS ===
void testFullRed() {
  if (!dma_display) return;
  dma_display->fillScreenRGB888(255, 0, 0);
}

void testFullGreen() {
  if (!dma_display) return;
  dma_display->fillScreenRGB888(0, 255, 0);
}

void testFullBlue() {
  if (!dma_display) return;
  dma_display->fillScreenRGB888(0, 0, 255);
}

void testFullWhite() {
  if (!dma_display) return;
  dma_display->fillScreenRGB888(255, 255, 255);
}

void testCornerPixels() {
  if (!dma_display) return;
  dma_display->clearScreen();
  // NOTE: Change these coordinates if using different matrix size
  // For 32x32: use (31, 31) instead of (63, 63)
  // For 64x32: use (63, 31) for bottom corners
  dma_display->drawPixel(0, 0, dma_display->color565(255, 0, 0));     // Top-left red
  dma_display->drawPixel(63, 0, dma_display->color565(0, 255, 0));    // Top-right green (63 for 64-wide)
  dma_display->drawPixel(0, 63, dma_display->color565(0, 0, 255));    // Bottom-left blue (63 for 64-tall)
  dma_display->drawPixel(63, 63, dma_display->color565(255, 255, 0)); // Bottom-right yellow
}

void testHorizontalLines() {
  if (!dma_display) return;
  dma_display->clearScreen();
  // NOTE: Change 64 to your matrix height (32 for 32-tall matrices)
  for(int y = 0; y < 64; y += 8) {
    dma_display->drawLine(0, y, 63, y, dma_display->color565(255, 255, 255)); // 63 = width-1
  }
}

void testVerticalLines() {
  if (!dma_display) return;
  dma_display->clearScreen();
  for(int x = 0; x < 64; x += 8) {
    dma_display->drawLine(x, 0, x, 63, dma_display->color565(255, 255, 255));
  }
}

void testBrightnessLevels() {
  if (!dma_display) return;
  static int brightLevel = 0;
  uint8_t levels[] = {255, 128, 64, 32, 16, 8};
  
  dma_display->setBrightness8(levels[brightLevel % 6]);
  dma_display->fillScreenRGB888(255, 255, 255);
  
  Serial.println("   Current brightness: " + String(levels[brightLevel % 6]) + "/255");
  brightLevel++;
}

void testColorGradient() {
  if (!dma_display) return;
  dma_display->clearScreen();
  
  // Red to Green gradient (top half)
  for(int y = 0; y < 32; y++) {
    int red = 255 - (y * 8);
    int green = y * 8;
    dma_display->drawLine(0, y, 63, y, dma_display->color565(red, green, 0));
  }
  
  // Green to Blue gradient (bottom half)
  for(int y = 32; y < 64; y++) {
    int green = 255 - ((y-32) * 8);
    int blue = (y-32) * 8;
    dma_display->drawLine(0, y, 63, y, dma_display->color565(0, green, blue));
  }
}

void testTextDisplay() {
  if (!dma_display) return;
  dma_display->clearScreen();
  dma_display->setTextSize(2);
  dma_display->setTextColor(dma_display->color565(255, 255, 0));
  dma_display->setCursor(8, 20);
  dma_display->print("TEST");
  dma_display->setCursor(15, 40);
  dma_display->print("OK");
}

// === MAIN AUTO DIAGNOSTIC RUNNER ===
void runAutoDiagnostics() {
  if (!displayFound) {
    static unsigned long lastRetry = 0;
    if (millis() - lastRetry > 10000) {  // Retry every 10 seconds
      printWarning("Display not found - retrying configuration detection...");
      findWorkingConfiguration();
      lastRetry = millis();
    }
    return;
  }
  
  // Check if it's time to start next test
  if (millis() - testStartTime >= TEST_DURATION) {
    currentTest = (currentTest + 1) % NUM_TESTS;
    testStartTime = millis();
    
    // Print test information
    printTestInfo(tests[currentTest].name, tests[currentTest].description);
    
    // Run the test
    tests[currentTest].testFunction();
    
    // Print timing info
    Serial.println("⏰ Test will run until: " + String((testStartTime + TEST_DURATION) / 1000) + "s");
  }
}

// === SETUP ===
void printWiringGuide() {
  printHeader("WIRING CHECKLIST - VERIFY ALL CONNECTIONS");
  Serial.println("📋 64x64 LED Matrix requires 16 wires + power + ground:");
  Serial.println("");
  
  Serial.println("🔴 RGB DATA WIRES (6 wires):");
  Serial.println("   Matrix R1 ← ESP32 Pin 42");
  Serial.println("   Matrix G1 ← ESP32 Pin 41"); 
  Serial.println("   Matrix B1 ← ESP32 Pin 40");
  Serial.println("   Matrix R2 ← ESP32 Pin 38");
  Serial.println("   Matrix G2 ← ESP32 Pin 39");
  Serial.println("   Matrix B2 ← ESP32 Pin 37");
  Serial.println("");
  
  Serial.println("📍 ADDRESS WIRES (5 wires):");
  Serial.println("   Matrix A ← ESP32 Pin 45");
  Serial.println("   Matrix B ← ESP32 Pin 48");
  Serial.println("   Matrix C ← ESP32 Pin 47");
  Serial.println("   Matrix D ← ESP32 Pin 21");
  Serial.println("   Matrix E ← ESP32 Pin 14");
  Serial.println("");
  
  Serial.println("⚡ CONTROL WIRES (3 wires):");
  Serial.println("   Matrix LAT ← ESP32 Pin 2");
  Serial.println("   Matrix OE ← ESP32 Pin 1");
  Serial.println("   Matrix CLK ← ESP32 Pin 36");
  Serial.println("");
  
  Serial.println("🔌 POWER CONNECTIONS:");
  Serial.println("   Matrix 5V ← External 5V Power Supply (2-4A)");
  Serial.println("   Matrix GND ← External PSU GND AND ESP32 GND");
  Serial.println("   ESP32 5V ← External 5V OR USB (separate from matrix)");
  Serial.println("");
  
  printWarning("CRITICAL: Matrix GND MUST connect to ESP32 GND!");
  printWarning("CRITICAL: Matrix needs 5V at 2-4A (USB is not enough!)");
  Serial.println("");
}

void checkBasicConnectivity() {
  printHeader("BASIC CONNECTIVITY TEST");
  
  // Test if we can even create the display object
  HUB75_I2S_CFG::i2s_pins pins = {
    .r1 = 42, .g1 = 41, .b1 = 40, .r2 = 38, .g2 = 39, .b2 = 37,
    .a = 45, .b = 48, .c = 47, .d = 21, .e = 14, .lat = 2, .oe = 1, .clk = 36
  };
  
  HUB75_I2S_CFG mxconfig(64, 64, 1, pins);
  mxconfig.double_buff = false;  // Simplest possible config
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::SHIFTREG;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_8M;  // Even slower
  
  MatrixPanel_I2S_DMA* testDisplay = new MatrixPanel_I2S_DMA(mxconfig);
  
  Serial.println("🧪 Testing basic I2S initialization...");
  if (testDisplay->begin()) {
    printStatus("I2S initialization SUCCESS");
    
    // Try the most basic possible output
    Serial.println("🧪 Testing basic pixel write...");
    testDisplay->setBrightness8(255);
    testDisplay->clearScreen();
    
    // Single pixel test
    testDisplay->drawPixel(0, 0, 0xFFFF);  // White pixel at 0,0
    delay(2000);
    
    // Full screen test
    testDisplay->fillScreenRGB888(255, 0, 0);  // Full red
    delay(2000);
    
    printStatus("Basic display commands sent - check matrix!");
  } else {
    printError("I2S initialization FAILED - hardware problem!");
  }
  
  delete testDisplay;
}

void setup() {
  Serial.begin(115200);
  delay(3000);  // Wait for serial monitor
  
  printHeader("ESP32-S3 LED MATRIX HARDWARE DIAGNOSTIC");
  Serial.println("🔥 NOTHING LIGHTING UP? Let's find the problem!");
  Serial.println("📋 Follow this step-by-step checklist:");
  Serial.println("");
  
  // First show wiring guide
  printWiringGuide();
  
  // Test basic connectivity
  checkBasicConnectivity();
  
  printHeader("AUTO DIAGNOSTIC SYSTEM STARTING");
  Serial.println("🤖 Watch for ANY light on the matrix during tests!");
  Serial.println("🔄 Tests run continuously - even a flicker means progress!");
  Serial.println("");
  
  printInfo("Auto diagnostics starting in 3 seconds...");
  delay(3000);
  
  // Find working display configuration
  findWorkingConfiguration();
  
  if (displayFound) {
    printStatus("READY: Starting automatic diagnostic tests!");
    testStartTime = millis();
  } else {
    printError("NO DISPLAY FOUND - Will keep retrying...");
  }
  
  printSeparator();
}

// === LOOP ===
void loop() {
  // Print heartbeat every 30 seconds
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 30000) {
    printSeparator();
    Serial.println("💓 SYSTEM HEARTBEAT - " + String(millis()/1000) + "s uptime");
    if (displayFound) {
      Serial.println("📺 Display: WORKING");
      Serial.println("🧪 Current test: " + tests[currentTest].name);
      Serial.println("⏱️  Next test in: " + String((TEST_DURATION - (millis() - testStartTime))/1000) + "s");
    } else {
      Serial.println("📺 Display: NOT FOUND - retrying...");
    }
    printSeparator();
    lastHeartbeat = millis();
  }
  
  // Run automatic diagnostics
  runAutoDiagnostics();
  
  // Small delay to prevent overwhelming
  delay(100);
}