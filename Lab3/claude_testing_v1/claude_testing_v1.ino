/**********************************************************************
 * ESP32 HUB75 Matrix Test - Standard ESP32 Pins
 * Simple test to verify matrix connection and power
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA* dma_display = nullptr;

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("\n================================");
  Serial.println("ESP32 HUB75 Matrix Connection Test");
  Serial.println("================================");
  
// // Configuration 2: Recommended Safe Pins (ESP32-S3, Freenove focused)
// PinConfig config2[] = {
//   {42, "R1", "Red Upper", "Blue"},     // Pin 1 (Keep working RGB)
//   {41, "G1", "Green Upper", "Green"},  // Pin 2 (Keep working RGB)
//   {40, "B1", "Blue Upper", "Yellow"},  // Pin 3 (Keep working RGB)
//   {39, "R2", "Red Lower", "Orange"},   // Pin 4 (Keep working RGB)
//   {38, "G2", "Green Lower", "Red"},    // Pin 5 (Keep working RGB)
//   {37, "B2", "Blue Lower", "Brown"},   // Pin 6 (Keep working RGB)
//   {45, "A", "Address A", "Black"},     // Pin 7 (Keep, generally fine)
//   {48, "B", "Address B", "White"},     // Pin 8 (Keep, generally fine)
//   {47, "C", "Address C", "Silver"},    // Pin 9 (Keep, generally fine)
//   {15, "D", "Address D", "Purple"},    // Pin 10 (Changed from 21: Safer GPIO)
//   {16, "E", "Address E", "Blue"},      // Pin 11 (Changed from 14: Safer GPIO)
//   {-1, "GND", "Ground", "Green"},      // Pin 12 (Ground)
//   {17, "LAT", "Latch", "Yellow"},      // Pin 13 (Changed from 2: Avoids UART TX)
//   {18, "OE", "Output Enable", "Orange"},// Pin 14 (Changed from 1: Avoids UART RX)
//   {19, "CLK", "Clock", "Red"},         // Pin 15 (Changed from 36: Safer, contiguous)
//   {-1, "5V", "Power 5V", "Brown"}      // Pin 16 (Power)

  // ESP32 S3 pins (see configuration above)
  HUB75_I2S_CFG::i2s_pins _pins = {
    .r1 = 42,  // Red 1
    .g1 = 41,  // Green 1  
    .b1 = 40,  // Blue 1
    .r2 = 39,  // Red 2
    .g2 = 38,  // Green 2
    .b2 = 37,  // Blue 2
    .a = 45,   // Address A
    .b = 48,   // Address B
    .c = 47,   // Address C
    .d = 15,   // Address D
    .e = 16,   // Address E (for 64x64, -1 for 32x32)
    .lat = 17,  // Latch
    .oe = 18,  // Output Enable
    .clk = 19  // Clock
  };

  Serial.println("[PINS] Standard ESP32 configuration:");
  Serial.println("R1=25, G1=26, B1=27");
  Serial.println("R2=14, G2=12, B2=13"); 
  Serial.println("A=23, B=19, C=5, D=17, E=18");
  Serial.println("LAT=4, OE=15, CLK=16");
  Serial.println("GND and 5V must also be connected!");
  
  // Create matrix configuration
  HUB75_I2S_CFG mxconfig(64, 64, 1, _pins);
  mxconfig.double_buff = false;  // Keep it simple
  mxconfig.clkphase = false;
  
  // Try different drivers
  Serial.println("\n[TEST 1] Trying SHIFTREG driver...");
  mxconfig.driver = HUB75_I2S_CFG::SHIFTREG;
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  
  if (dma_display->begin()) {
    Serial.println("[SUCCESS] Matrix initialized with SHIFTREG driver!");
    runDisplayTest();
  } else {
    Serial.println("[FAILED] SHIFTREG failed, trying FM6126A...");
    delete dma_display;
    
    // Try FM6126A driver
    mxconfig.driver = HUB75_I2S_CFG::FM6126A;
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    
    if (dma_display->begin()) {
      Serial.println("[SUCCESS] Matrix initialized with FM6126A driver!");
      runDisplayTest();
    } else {
      Serial.println("[CRITICAL] Both drivers failed!");
      Serial.println("\nTROUBLESHOOTING:");
      Serial.println("1. Check 5V power supply (at least 2A)");
      Serial.println("2. Verify all 16 wires are connected");
      Serial.println("3. Check ground connection");
      Serial.println("4. Confirm matrix is 64x64 HUB75 type");
      dma_display = nullptr;
    }
  }
}

void runDisplayTest() {
  if (dma_display == nullptr) return;
  
  Serial.println("\n[DISPLAY TEST] Starting visual tests...");
  
  // Test 1: Full brightness red
  Serial.println("TEST 1: Bright red screen (3 seconds)");
  dma_display->setBrightness8(255);  // Maximum brightness
  dma_display->fillScreenRGB888(255, 0, 0);
  delay(3000);
  
  // Test 2: Full brightness green
  Serial.println("TEST 2: Bright green screen (3 seconds)");
  dma_display->fillScreenRGB888(0, 255, 0);
  delay(3000);
  
  // Test 3: Full brightness blue
  Serial.println("TEST 3: Bright blue screen (3 seconds)");
  dma_display->fillScreenRGB888(0, 0, 255);
  delay(3000);
  
  // Test 4: White
  Serial.println("TEST 4: Bright white screen (3 seconds)");
  dma_display->fillScreenRGB888(255, 255, 255);
  delay(3000);
  
  // Test 5: Clear screen
  Serial.println("TEST 5: Clear screen (black)");
  dma_display->clearScreen();
  delay(2000);
  
  // Test 6: Single pixel test
  Serial.println("TEST 6: Single white pixel at center");
  dma_display->drawPixel(32, 32, dma_display->color565(255, 255, 255));
  delay(3000);
  
  // Test 7: Simple pattern
  Serial.println("TEST 7: Grid pattern");
  dma_display->clearScreen();
  for(int x = 0; x < 64; x += 8) {
    for(int y = 0; y < 64; y += 8) {
      dma_display->drawPixel(x, y, dma_display->color565(255, 255, 0));
    }
  }
  delay(3000);
  
  // Test 8: Text
  Serial.println("TEST 8: Text display");
  dma_display->clearScreen();
  dma_display->setTextSize(2);
  dma_display->setTextColor(dma_display->color565(0, 255, 255));
  dma_display->setCursor(5, 25);
  dma_display->print("ESP32");
  delay(3000);
  
  // Test 9: Animation
  Serial.println("TEST 9: Simple animation");
  for(int i = 0; i < 20; i++) {
    dma_display->clearScreen();
    dma_display->fillRect(i*3, i*3, 8, 8, dma_display->color565(255, 0, 255));
    delay(100);
  }
  
  Serial.println("\n[COMPLETE] All tests finished!");
  Serial.println("If you saw colors on the matrix, it's working!");
  Serial.println("If not, check power supply and wiring.");
  
  // Set to dim for continuous operation
  dma_display->setBrightness8(50);
  dma_display->clearScreen();
}

void loop() {
  if (dma_display == nullptr) {
    Serial.println("[ERROR] Matrix not initialized - check setup messages");
    delay(5000);
    return;
  }
  
  // Simple cycling pattern to show it's working
  static int colorStep = 0;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate > 2000) {
    switch(colorStep % 4) {
      case 0:
        dma_display->fillScreenRGB888(50, 0, 0);  // Dim red
        break;
      case 1:
        dma_display->fillScreenRGB888(0, 50, 0);  // Dim green
        break;
      case 2:
        dma_display->fillScreenRGB888(0, 0, 50);  // Dim blue
        break;
      case 3:
        dma_display->clearScreen();               // Black
        break;
    }
    colorStep++;
    lastUpdate = millis();
  }
}