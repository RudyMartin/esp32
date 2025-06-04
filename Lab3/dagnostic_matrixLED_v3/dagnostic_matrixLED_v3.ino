/**********************************************************************
 * Filename    : RedLines_Troubleshoot.ino
 * Description : Fix red lines issue and get full matrix control
 * Author      : Rudy Martin / Next Shift Consulting
 * Project     : Artemis DSAI 2025 – LED Matrix Display Labs Using Agents
 * Modified    : Jun 4, 2025   
 * PURPOSE: You see red lines - let's get full color and movement!
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA* dma_display = nullptr;
bool displayWorking = false;
int testMode = 0;
unsigned long lastTestChange = 0;
const unsigned long TEST_INTERVAL = 3000;  // 3 seconds per test

void printProgress(String message) {
  Serial.println("🔍 " + message);
}

void printSuccess(String message) {
  Serial.println("✅ " + message);
}

void printIssue(String message) {
  Serial.println("⚠️  " + message);
}

void initializeDisplay() {
  Serial.println("\n🚀 INITIALIZING DISPLAY (You already saw red lines - good sign!)");
  Serial.println("================================================================");
  
  // Use the pin configuration that gave you red lines
  HUB75_I2S_CFG::i2s_pins pins = {
    .r1 = 42, .g1 = 41, .b1 = 40,
    .r2 = 38, .g2 = 39, .b2 = 37,
    .a = 45, .b = 48, .c = 47, .d = 21, .e = 14,
    .lat = 2, .oe = 1, .clk = 36
  };

  // Try different configuration settings to fix the red lines issue
  HUB75_I2S_CFG mxconfig(64, 64, 1, pins);
  mxconfig.double_buff = true;           // Try double buffering
  mxconfig.clkphase = false;             // Try different clock phase
  mxconfig.driver = HUB75_I2S_CFG::SHIFTREG;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;  // Try faster speed
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  
  if (dma_display->begin()) {
    printSuccess("Display initialized - fixing red lines issue...");
    
    // Critical: Set brightness and clear immediately
    dma_display->setBrightness8(40);  // Safe brightness
    dma_display->clearScreen();
    delay(500);
    
    displayWorking = true;
    return;
  }
  
  // If that didn't work, try different settings
  Serial.println("🔧 Trying alternative configuration...");
  delete dma_display;
  
  mxconfig.double_buff = false;          // Try without double buffering
  mxconfig.clkphase = true;              // Try different clock phase
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;  // Try different driver
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  
  if (dma_display->begin()) {
    printSuccess("Display initialized with alternative settings");
    dma_display->setBrightness8(40);
    dma_display->clearScreen();
    delay(500);
    displayWorking = true;
  } else {
    Serial.println("❌ Could not fix display configuration");
  }
}

// === PROGRESSIVE TESTS TO FIX THE RED LINES ===
void testClearScreen() {
  printProgress("TEST 1: Clear screen (should remove red lines)");
  if (!dma_display) return;
  
  dma_display->clearScreen();
  Serial.println("   LOOK FOR: Completely black/dark screen");
  Serial.println("   If still red lines: Address line issue");
}

void testSinglePixels() {
  printProgress("TEST 2: Single pixels in different colors");
  if (!dma_display) return;
  
  dma_display->clearScreen();
  delay(500);
  
  // Test individual color channels
  dma_display->drawPixel(5, 5, dma_display->color565(255, 0, 0));   // Red
  dma_display->drawPixel(10, 10, dma_display->color565(0, 255, 0)); // Green  
  dma_display->drawPixel(15, 15, dma_display->color565(0, 0, 255)); // Blue
  dma_display->drawPixel(20, 20, dma_display->color565(255, 255, 255)); // White
  
  Serial.println("   LOOK FOR: Red, Green, Blue, White pixels");
  Serial.println("   If only red shows: Green/Blue data lines issue");
}

void testHorizontalLines() {
  printProgress("TEST 3: Horizontal lines test");
  if (!dma_display) return;
  
  dma_display->clearScreen();
  delay(500);
  
  // Draw lines in different colors
  dma_display->drawLine(0, 10, 63, 10, dma_display->color565(255, 0, 0));   // Red line
  dma_display->drawLine(0, 20, 63, 20, dma_display->color565(0, 255, 0));   // Green line
  dma_display->drawLine(0, 30, 63, 30, dma_display->color565(0, 0, 255));   // Blue line
  dma_display->drawLine(0, 40, 63, 40, dma_display->color565(255, 255, 0)); // Yellow line
  
  Serial.println("   LOOK FOR: Red, Green, Blue, Yellow horizontal lines");
  Serial.println("   If lines are wrong colors: Color data wiring issue");
}

void testVerticalLines() {
  printProgress("TEST 4: Vertical lines test");
  if (!dma_display) return;
  
  dma_display->clearScreen();
  delay(500);
  
  // Draw vertical lines
  dma_display->drawLine(10, 0, 10, 63, dma_display->color565(255, 0, 0));   // Red
  dma_display->drawLine(20, 0, 20, 63, dma_display->color565(0, 255, 0));   // Green
  dma_display->drawLine(30, 0, 30, 63, dma_display->color565(0, 0, 255));   // Blue
  dma_display->drawLine(40, 0, 40, 63, dma_display->color565(255, 255, 255)); // White
  
  Serial.println("   LOOK FOR: Red, Green, Blue, White vertical lines");
  Serial.println("   Tests if row addressing works correctly");
}

void testQuadrants() {
  printProgress("TEST 5: Four colored quadrants");
  if (!dma_display) return;
  
  dma_display->clearScreen();
  delay(500);
  
  // Fill quadrants with different colors
  for(int x = 0; x < 32; x++) {
    for(int y = 0; y < 32; y++) {
      dma_display->drawPixel(x, y, dma_display->color565(255, 0, 0)); // Top-left red
    }
  }
  
  for(int x = 32; x < 64; x++) {
    for(int y = 0; y < 32; y++) {
      dma_display->drawPixel(x, y, dma_display->color565(0, 255, 0)); // Top-right green
    }
  }
  
  for(int x = 0; x < 32; x++) {
    for(int y = 32; y < 64; y++) {
      dma_display->drawPixel(x, y, dma_display->color565(0, 0, 255)); // Bottom-left blue
    }
  }
  
  for(int x = 32; x < 64; x++) {
    for(int y = 32; y < 64; y++) {
      dma_display->drawPixel(x, y, dma_display->color565(255, 255, 0)); // Bottom-right yellow
    }
  }
  
  Serial.println("   LOOK FOR: Red(TL), Green(TR), Blue(BL), Yellow(BR) squares");
  Serial.println("   Tests full matrix control and all colors");
}

void testMovingPixel() {
  printProgress("TEST 6: Moving pixel animation");
  if (!dma_display) return;
  
  static int pixelX = 0;
  static int pixelY = 0;
  static unsigned long lastMove = 0;
  
  if (millis() - lastMove > 100) {  // Move every 100ms
    dma_display->clearScreen();
    
    // Draw moving white pixel
    dma_display->drawPixel(pixelX, pixelY, dma_display->color565(255, 255, 255));
    
    // Move pixel in square pattern
    if (pixelY == 0 && pixelX < 63) pixelX++;          // Top edge
    else if (pixelX == 63 && pixelY < 63) pixelY++;    // Right edge
    else if (pixelY == 63 && pixelX > 0) pixelX--;     // Bottom edge
    else if (pixelX == 0 && pixelY > 0) pixelY--;      // Left edge
    
    lastMove = millis();
  }
  
  Serial.println("   LOOK FOR: White pixel moving around the border");
  Serial.println("   This tests animation and matrix refresh");
}

void testBrightnessFade() {
  printProgress("TEST 7: Brightness fade test");
  if (!dma_display) return;
  
  static int brightness = 10;
  static int direction = 1;
  static unsigned long lastFade = 0;
  
  if (millis() - lastFade > 50) {  // Change brightness every 50ms
    dma_display->setBrightness8(brightness);
    dma_display->fillScreenRGB888(0, 255, 0);  // Green screen
    
    brightness += direction * 5;
    if (brightness >= 100) direction = -1;
    if (brightness <= 10) direction = 1;
    
    lastFade = millis();
  }
  
  Serial.println("   LOOK FOR: Green screen fading bright and dim");
  Serial.println("   Tests brightness control and full screen");
}

void testText() {
  printProgress("TEST 8: Text display test");
  if (!dma_display) return;
  
  dma_display->clearScreen();
  dma_display->setTextSize(1);
  dma_display->setTextColor(dma_display->color565(255, 255, 0)); // Yellow text
  dma_display->setCursor(5, 5);
  dma_display->print("MATRIX");
  dma_display->setCursor(10, 20);
  dma_display->print("WORKS!");
  dma_display->setCursor(15, 35);
  dma_display->print("64x64");
  
  Serial.println("   LOOK FOR: Yellow text saying 'MATRIX WORKS! 64x64'");
  Serial.println("   Tests text rendering and precise pixel control");
}

// === WIRING DIAGNOSIS ===
void diagnoseRedLines() {
  Serial.println("\n🔧 RED LINES DIAGNOSIS:");
  Serial.println("================================================================");
  Serial.println("You're seeing red lines, which typically means:");
  Serial.println("");
  Serial.println("✅ GOOD NEWS:");
  Serial.println("   - Power supply is working");
  Serial.println("   - ESP32 is working");
  Serial.println("   - Red data lines (R1, R2) are connected");
  Serial.println("   - Clock and control lines partially working");
  Serial.println("");
  Serial.println("🔧 LIKELY ISSUES:");
  Serial.println("   - Green data lines (G1, G2) not connected properly");
  Serial.println("   - Blue data lines (B1, B2) not connected properly");
  Serial.println("   - Address lines (A, B, C, D, E) may have issues");
  Serial.println("   - LAT or OE control lines may need adjustment");
  Serial.println("");
  Serial.println("📋 CHECK THESE CONNECTIONS:");
  Serial.println("   Matrix G1 ← ESP32 Pin 41 (Green top half)");
  Serial.println("   Matrix G2 ← ESP32 Pin 39 (Green bottom half)");
  Serial.println("   Matrix B1 ← ESP32 Pin 40 (Blue top half)");
  Serial.println("   Matrix B2 ← ESP32 Pin 37 (Blue bottom half)");
  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("🎉 GREAT NEWS: You saw red lines!");
  Serial.println("================================================================");
  Serial.println("This means your matrix and ESP32 are working!");
  Serial.println("Now let's fix the red lines and get full control...");
  Serial.println("");
  
  diagnoseRedLines();
  delay(3}