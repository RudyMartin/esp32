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
  
  // Standard ESP32 pins (most common configuration)
  HUB75_I2S_CFG::i2s_pins _pins = {
    .r1 = 25,  // Red 1
    .g1 = 26,  // Green 1  
    .b1 = 27,  // Blue 1
    .r2 = 14,  // Red 2
    .g2 = 12,  // Green 2
    .b2 = 13,  // Blue 2
    .a = 23,   // Address A
    .b = 19,   // Address B
    .c = 5,    // Address C
    .d = 17,   // Address D
    .e = 18,   // Address E (for 64x64, -1 for 32x32)
    .lat = 4,  // Latch
    .oe = 15,  // Output Enable
    .clk = 16  // Clock
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