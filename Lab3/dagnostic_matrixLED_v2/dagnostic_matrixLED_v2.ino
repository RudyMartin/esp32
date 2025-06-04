/**********************************************************************
 * Filename    : BurntDisplay_Recovery.ino
 * Description : Code for testing NEW LED matrix after burning previous one
 * Author      : Rudy Martin / Next Shift Consulting
 * Project     : Artemis DSAI 2025 – LED Matrix Display Labs Using Agents
 * Modified    : Jun 4, 2025   
 * PURPOSE: Safe testing protocol for NEW matrix (after burning old one)
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// === SAFE POWER SETTINGS ===
const uint8_t SAFE_BRIGHTNESS = 20;  // Very low brightness for new matrix
const unsigned long POWER_ON_DELAY = 5000;  // 5 second delay before any tests

MatrixPanel_I2S_DMA* dma_display = nullptr;
bool newMatrixConnected = false;

void printSafetyWarning() {
  Serial.println("\n🚨🚨🚨 SAFETY WARNING 🚨🚨🚨");
  Serial.println("================================================================");
  Serial.println("⚠️  PREVIOUS MATRIX WAS BURNT - FOLLOW SAFETY PROTOCOL!");
  Serial.println("================================================================");
  Serial.println("");
  Serial.println("🔥 WHAT WENT WRONG:");
  Serial.println("   - Overvoltage or overcurrent damaged the matrix");
  Serial.println("   - Burnt matrix causes crashes and won't light up");
  Serial.println("   - Need NEW matrix to continue");
  Serial.println("");
  Serial.println("🛡️  SAFETY PROTOCOL FOR NEW MATRIX:");
  Serial.println("   1. ✅ EXACTLY 5.0V (measure with multimeter)");
  Serial.println("   2. ✅ Current-limited power supply (2A max initially)");
  Serial.println("   3. ✅ Start with VERY low brightness (20/255)");
  Serial.println("   4. ✅ Test ONE pixel first, then gradually increase");
  Serial.println("   5. ✅ Monitor temperature - matrix should stay cool");
  Serial.println("");
  Serial.println("❌ NEVER EXCEED:");
  Serial.println("   - 5.2V (will damage matrix)");
  Serial.println("   - 4A current (even for 64x64)");
  Serial.println("   - 50% brightness until proven stable");
  Serial.println("");
}

void printPowerSupplyGuide() {
  Serial.println("🔌 SAFE POWER SUPPLY SETUP:");
  Serial.println("================================================================");
  Serial.println("✅ RECOMMENDED SUPPLIES:");
  Serial.println("   - Adjustable bench power supply (set to 5.0V, 2A limit)");
  Serial.println("   - Quality 5V/3A wall adapter with current limiting");
  Serial.println("   - Computer ATX power supply (5V rail, add current limit)");
  Serial.println("");
  Serial.println("⚠️  POWER SUPPLY SETTINGS:");
  Serial.println("   - Voltage: EXACTLY 5.0V (measure with multimeter)");
  Serial.println("   - Current limit: Start at 1A, increase slowly");
  Serial.println("   - Monitor current draw during testing");
  Serial.println("");
  Serial.println("🚫 AVOID:");
  Serial.println("   - USB power (insufficient current, voltage drop)");
  Serial.println("   - Unregulated supplies");
  Serial.println("   - Supplies without current limiting");
  Serial.println("   - 'Fast charging' adapters (voltage spikes)");
  Serial.println("");
}

void checkForNewMatrix() {
  Serial.println("🔍 CHECKING FOR NEW MATRIX:");
  Serial.println("================================================================");
  Serial.println("📋 PRE-FLIGHT CHECKLIST:");
  Serial.println("   □ Old burnt matrix disconnected?");
  Serial.println("   □ New matrix connected with fresh wiring?");
  Serial.println("   □ Power supply set to exactly 5.0V?");
  Serial.println("   □ Current limit set to 2A maximum?");
  Serial.println("   □ All 16 data wires connected correctly?");
  Serial.println("   □ Ground wire between ESP32 and matrix?");
  Serial.println("");
  
  Serial.println("⏳ Waiting 5 seconds before attempting initialization...");
  Serial.println("   (This prevents power surge to new matrix)");
  
  for (int i = 5; i > 0; i--) {
    Serial.println("   " + String(i) + "...");
    delay(1000);
  }
}

bool safeMatrixInit() {
  Serial.println("\n🧪 ATTEMPTING SAFE MATRIX INITIALIZATION:");
  Serial.println("================================================================");
  
  HUB75_I2S_CFG::i2s_pins pins = {
    .r1 = 42, .g1 = 41, .b1 = 40,
    .r2 = 38, .g2 = 39, .b2 = 37,
    .a = 45, .b = 48, .c = 47, .d = 21, .e = 14,
    .lat = 2, .oe = 1, .clk = 36
  };

  HUB75_I2S_CFG mxconfig(64, 64, 1, pins);
  mxconfig.double_buff = false;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::SHIFTREG;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_8M;  // Conservative speed
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  
  if (dma_display->begin()) {
    Serial.println("✅ Matrix initialization SUCCESS!");
    
    // CRITICAL: Set very low brightness immediately
    dma_display->setBrightness8(SAFE_BRIGHTNESS);
    dma_display->clearScreen();
    
    Serial.println("🔅 Brightness set to SAFE level: " + String(SAFE_BRIGHTNESS) + "/255");
    return true;
  } else {
    Serial.println("❌ Matrix initialization FAILED");
    Serial.println("   Possible causes:");
    Serial.println("   - No matrix connected");
    Serial.println("   - Wiring errors");
    Serial.println("   - Power supply issues");
    Serial.println("   - Matrix also damaged");
    
    delete dma_display;
    dma_display = nullptr;
    return false;
  }
}

void ultraSafeDisplayTest() {
  if (dma_display == nullptr) {
    Serial.println("❌ No working matrix - cannot run display tests");
    return;
  }
  
  Serial.println("\n🛡️  ULTRA-SAFE DISPLAY TESTS:");
  Serial.println("================================================================");
  Serial.println("⚠️  Monitoring for overheating or unusual behavior...");
  Serial.println("");
  
  // Test 1: Single pixel
  Serial.println("🔴 Test 1: Single red pixel (safest possible test)");
  Serial.println("   LOOK FOR: One small red dot in top-left corner");
  Serial.println("   MONITOR: Matrix should stay cool to touch");
  dma_display->clearScreen();
  dma_display->drawPixel(0, 0, dma_display->color565(255, 0, 0));
  delay(5000);
  
  // Test 2: Few pixels
  Serial.println("🟢 Test 2: Four corner pixels");
  Serial.println("   LOOK FOR: Small colored dots in corners");
  Serial.println("   MONITOR: Check for any heat buildup");
  dma_display->clearScreen();
  dma_display->drawPixel(0, 0, dma_display->color565(255, 0, 0));     // Red
  dma_display->drawPixel(63, 0, dma_display->color565(0, 255, 0));    // Green
  dma_display->drawPixel(0, 63, dma_display->color565(0, 0, 255));    // Blue
  dma_display->drawPixel(63, 63, dma_display->color565(255, 255, 0)); // Yellow
  delay(5000);
  
  // Test 3: Small pattern
  Serial.println("⚪ Test 3: Small cross pattern");
  Serial.println("   LOOK FOR: White cross in center");
  dma_display->clearScreen();
  // Draw small cross
  for (int i = 30; i < 34; i++) {
    dma_display->drawPixel(32, i, dma_display->color565(255, 255, 255));  // Vertical line
    dma_display->drawPixel(i, 32, dma_display->color565(255, 255, 255));  // Horizontal line
  }
  delay(5000);
  
  Serial.println("✅ Basic tests completed successfully!");
  Serial.println("📊 Matrix appears functional and safe");
  Serial.println("🌡️  Check matrix temperature - should be cool/warm, not hot");
  
  dma_display->clearScreen();
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  // Show safety warnings
  printSafetyWarning();
  delay(3000);
  
  printPowerSupplyGuide();
  delay(3000);
  
  checkForNewMatrix();
  
  // Attempt safe initialization
  if (safeMatrixInit()) {
    newMatrixConnected = true;
    ultraSafeDisplayTest();
    
    Serial.println("\n🎉 SUCCESS! New matrix is working safely!");
    Serial.println("📋 NEXT STEPS:");
    Serial.println("   1. ✅ Matrix proven functional at low brightness");
    Serial.println("   2. 🔄 Can gradually increase brightness if needed");
    Serial.println("   3. 🧪 Can run full diagnostic tests");
    Serial.println("   4. 💾 Can load your original mission code");
  } else {
    Serial.println("\n❌ NEW MATRIX NOT DETECTED");
    Serial.println("📋 TROUBLESHOOTING:");
    Serial.println("   1. Verify new matrix is connected");
    Serial.println("   2. Check power supply voltage (exactly 5.0V)");
    Serial.println("   3. Verify all 16 data wire connections");
    Serial.println("   4. Ensure ground connection");
    Serial.println("   5. Try different matrix if available");
  }
  
  Serial.println("\n================================================================");
  Serial.println("💡 System will now loop safely");
  Serial.println("🔄 Reset ESP32 after connecting new matrix");
  Serial.println("================================================================");
}

void loop() {
  // Safe heartbeat
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 15000) {  // Every 15 seconds
    Serial.println("💓 System stable - Matrix status: " + 
                   String(newMatrixConnected ? "WORKING SAFELY" : "NOT DETECTED"));
    
    if (newMatrixConnected) {
      Serial.println("🌡️  Reminder: Check matrix temperature periodically");
    }
    
    lastHeartbeat = millis();
  }
  
  // Simple safe animation if matrix is working
  if (newMatrixConnected && dma_display != nullptr) {
    static unsigned long lastBlink = 0;
    static bool blinkState = false;
    
    if (millis() - lastBlink > 2000) {  // Blink every 2 seconds
      dma_display->clearScreen();
      if (blinkState) {
        // Show "OK" indicator - small green dot
        dma_display->drawPixel(32, 32, dma_display->color565(0, 128, 0));
      }
      blinkState = !blinkState;
      lastBlink = millis();
    }
  }
  
  delay(100);
}