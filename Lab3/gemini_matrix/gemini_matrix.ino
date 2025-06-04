#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Configuration for 64x64 matrix display
HUB75_I2S_CFG config;

// Display object pointer
MatrixPanel_I2S_DMA *dma_display = nullptr;

void configureMatrix() {
  Serial.println("\n--- Configuring Matrix Pin Assignments ---");
  // Set panel dimensions
  config.mx_width = 64;   // Panel width
  config.mx_height = 64;  // Panel height
  config.chain_length = 1; // Number of panels chained
  config.i2sspeed = HUB75_I2S_CFG::HZ_10M; // Slower speed for stability
  
  // Configure GPIO pins - VERIFY these match your actual wiring!
  // Critical for 64x64 panels: A, B, C, D, E address lines must be correct.
  // Data lines R1, G1, B1, R2, G2, B2 must be connected.
  // Control lines LAT, OE, CLK must be connected.

  config.gpio.r1 = 25;   Serial.printf("  R1 (Red Upper): GPIO%d\n", config.gpio.r1);
  config.gpio.g1 = 26;   Serial.printf("  G1 (Green Upper): GPIO%d\n", config.gpio.g1);
  config.gpio.b1 = 27;   Serial.printf("  B1 (Blue Upper): GPIO%d\n", config.gpio.b1);
  config.gpio.r2 = 14;   Serial.printf("  R2 (Red Lower): GPIO%d\n", config.gpio.r2);
  config.gpio.g2 = 12;   Serial.printf("  G2 (Green Lower): GPIO%d\n", config.gpio.g2);
  config.gpio.b2 = 13;   Serial.printf("  B2 (Blue Lower): GPIO%d\n", config.gpio.b2);
  
  config.gpio.a = 23;    Serial.printf("  A (Address Line): GPIO%d\n", config.gpio.a);
  config.gpio.b = 22;    Serial.printf("  B (Address Line): GPIO%d\n", config.gpio.b);
  config.gpio.c = 5;     Serial.printf("  C (Address Line): GPIO%d\n", config.gpio.c);
  config.gpio.d = 17;    Serial.printf("  D (Address Line): GPIO%d\n", config.gpio.d);
  config.gpio.e = 33;    Serial.printf("  E (Address Line - CRITICAL for 64-height): GPIO%d\n", config.gpio.e); // Emphasize E
  
  config.gpio.lat = 4;   Serial.printf("  LAT (Latch): GPIO%d\n", config.gpio.lat);
  config.gpio.oe = 15;   Serial.printf("  OE (Output Enable): GPIO%d\n", config.gpio.oe);
  config.gpio.clk = 16;  Serial.printf("  CLK (Clock): GPIO%d\n", config.gpio.clk);

  Serial.println("--- Pin assignment configured. Double-check your wiring to match! ---");
}

void setup() {
  Serial.begin(115200);
  delay(10000);
  Serial.println("\n=== 64x64 ESP32 Matrix Display Test Sketch ===");
  Serial.println(">> Uploading and running... pay close attention to Serial Monitor AND Matrix Display.");

  // Configure the matrix settings
  configureMatrix();
  
  // Create display object
  Serial.println("\nAttempting to create display object...");
  dma_display = new MatrixPanel_I2S_DMA(config);

  // Initialize display
  Serial.println("Attempting to initialize display hardware...");
  if (!dma_display->begin()) {
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.println("ERROR: Display initialization FAILED!");
    Serial.println("Possible causes:");
    Serial.println("  - Incorrect pin configuration in the code.");
    Serial.println("  - Your ESP32 GPIOs may have conflicting uses.");
    Serial.println("  - Issues with the MatrixPanel_I2S_DMA library installation.");
    Serial.println("  - Hardware fault or severe wiring error (e.g., short circuit).");
    Serial.println("  - Not enough PSRAM for buffer (unlikely for 64x64 unless using other heavy libs).");
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    while(1) {
      delay(1000);
      Serial.println("STUCK: Please check serial output above for clues and re-upload.");
    }
  }

  Serial.println("\nSUCCESS: Display initialized successfully!");
  Serial.println(">> If the screen is still COMPLETELY BLANK, check your OE pin (GPIO15) connection and power supply.");

  // Set brightness and clear screen
  dma_display->setBrightness8(80);  // Medium brightness
  dma_display->clearScreen();
  
  // Run diagnostic tests
  runDiagnosticTests();
}

void runDiagnosticTests() {
  Serial.println("\n=== Starting Diagnostic Tests ===");
  Serial.println(">> Observe the matrix display carefully for each test described below.");
  Serial.println("---------------------------------------------------------------------");
  
  // Test 1: Clear screen (should be black)
  Serial.println("Test 1 (Wait 2s): Clear screen.");
  Serial.println("  EXPECT: Screen should be completely BLACK. No lights, no dots, no lines.");
  Serial.println("  If you see *anything* here, even faint, check power supply ripple or data/address line integrity.");
  dma_display->fillScreen(0);
  delay(2000);
  
  // Test 2: Fill with dim white
  Serial.println("Test 2 (Wait 3s): Dim WHITE screen (RGB 64,64,64).");
  Serial.println("  EXPECT: A uniform, solid, dim white across the ENTIRE 64x64 panel.");
  Serial.println("  If you see a dotted line, partial display (e.g., top half only), or flickering:");
  Serial.println("    -> Strongly suspect an ADDRESS LINE issue, especially E (GPIO33) for 64-height.");
  Serial.println("    -> Also check D (GPIO17), C (GPIO5), B (GPIO22), A (GPIO23) connections.");
  Serial.println("    -> And general power supply stability.");
  dma_display->fillScreen(dma_display->color565(64, 64, 64));
  delay(3000);
  
  // Test 3: Red screen
  Serial.println("Test 3 (Wait 3s): Full RED screen.");
  Serial.println("  EXPECT: A uniform, solid, bright red across the ENTIRE 64x64 panel.");
  Serial.println("  If color is incorrect (e.g., green instead of red), check R1/G1/B1/R2/G2/B2 data lines.");
  Serial.println("  If partial/dotted line, re-check address lines (A,B,C,D,E).");
  dma_display->fillScreen(dma_display->color565(255, 0, 0));
  delay(3000);
  
  // Test 4: Green screen
  Serial.println("Test 4 (Wait 3s): Full GREEN screen.");
  Serial.println("  EXPECT: A uniform, solid, bright green across the ENTIRE 64x64 panel.");
  Serial.println("  If color is incorrect, check R1/G1/B1/R2/G2/B2 data lines.");
  Serial.println("  If partial/dotted line, re-check address lines (A,B,C,D,E).");
  dma_display->fillScreen(dma_display->color565(0, 255, 0));
  delay(3000);
  
  // Test 5: Blue screen
  Serial.println("Test 5 (Wait 3s): Full BLUE screen.");
  Serial.println("  EXPECT: A uniform, solid, bright blue across the ENTIRE 64x64 panel.");
  Serial.println("  If color is incorrect, check R1/G1/B1/R2/G2/B2 data lines.");
  Serial.println("  If partial/dotted line, re-check address lines (A,B,C,D,E).");
  dma_display->fillScreen(dma_display->color565(0, 0, 255));
  delay(3000);
  
  // Test 6: Corner pixels
  Serial.println("Test 6 (Indefinite): Corner and center pixels.");
  Serial.println("  EXPECT: 5 distinct, small pixels on the screen:");
  Serial.println("    - Top-Left (0,0): WHITE");
  Serial.println("    - Top-Right (63,0): RED");
  Serial.println("    - Bottom-Left (0,63): GREEN");
  Serial.println("    - Bottom-Right (63,63): BLUE");
  Serial.println("    - Center (32,32): YELLOW");
  Serial.println("  If pixels are missing, wrong color, or misplaced:");
  Serial.println("    -> Re-check ALL data (R1/R2/G1/G2/B1/B2) and address (A,B,C,D,E) pins.");
  Serial.println("    -> For 64x64, *especially* if bottom half pixels are missing/wrong, verify E pin (GPIO33)!");
  dma_display->fillScreen(0);
  dma_display->drawPixel(0, 0, dma_display->color565(255, 255, 255));      // Top-left
  dma_display->drawPixel(63, 0, dma_display->color565(255, 0, 0));         // Top-right red
  dma_display->drawPixel(0, 63, dma_display->color565(0, 255, 0));         // Bottom-left green
  dma_display->drawPixel(63, 63, dma_display->color565(0, 0, 255));        // Bottom-right blue
  dma_display->drawPixel(32, 32, dma_display->color565(255, 255, 0));      // Center yellow
  delay(5000); // Give time to observe pixels

  Serial.println("---------------------------------------------------------------------");
  Serial.println("\n=== Diagnostic Tests Complete ===");
  Serial.println("Now the loop will run, switching patterns every 2 seconds.");
  Serial.println("FINAL QUESTION FOR YOU: Based on the tests and above messages:");
  Serial.println("  A. Did all tests show perfect solid colors and correctly placed pixels?");
  Serial.println("  B. Did you *only* see a dotted line or partial/scrambled display throughout?");
  Serial.println("     -> If B: The PROBLEM IS ALMOST CERTAINLY an ADDRESS LINE (A,B,C,D,E) wiring issue,");
  Serial.println("        or fundamental timing/power problem. Re-check E (GPIO33) first, then D, C, B, A.");
  Serial.println("  C. Were colors wrong (e.g., red when it should be green) or flickering erratically?");
  Serial.println("     -> If C: Check the DATA LINE (R1/R2, G1/G2, B1/B2) connections and your power supply.");
  Serial.println("  D. Was the entire display completely blank after 'Display initialized successfully!'?");
  Serial.println("     -> If D: Check the OE (Output Enable, GPIO15) pin connection and your power supply.");
  Serial.println("\nAlways ensure you have a robust 5V power supply for the matrix, separate from ESP32 power!");
  Serial.println("---------------------------------------------------------------------");
}

void loop() {
  // Simple alternating pattern to help identify issues
  static bool toggle = false;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate > 2000) {  // Every 2 seconds
    if (toggle) {
      // Draw checkerboard pattern
      Serial.println("Alternating pattern: CHECKERBOARD. EXPECT: A uniform checkerboard across the whole panel.");
      for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
          if ((x + y) % 2 == 0) {
            dma_display->drawPixel(x, y, dma_display->color565(100, 100, 100)); // Dim white for visibility
          } else {
            dma_display->drawPixel(x, y, 0); // Black
          }
        }
      }
    } else {
      // Draw horizontal lines
      Serial.println("Alternating pattern: HORIZONTAL LINES. EXPECT: Evenly spaced lines across the whole panel.");
      Serial.println("  If lines look broken, skewed, or if the pattern doesn't fill the whole 64x64,");
      Serial.println("  re-check all Address Lines (A,B,C,D,E) especially E (GPIO33).");
      dma_display->fillScreen(0);
      for (int y = 0; y < 64; y += 4) { // Draw a line every 4 rows
        for (int x = 0; x < 64; x++) {
          dma_display->drawPixel(x, y, dma_display->color565(150, 150, 150)); // Medium white line
        }
      }
    }
    
    toggle = !toggle;
    lastUpdate = millis();
  }
  
  delay(10);
}