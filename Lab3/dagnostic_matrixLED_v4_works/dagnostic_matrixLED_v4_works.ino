/**********************************************************************
 * Filename    : Deep_Dive_RedLines_AutoPilot.ino
 * Description : Automated, looped troubleshooting for persistent red lines on 64x64 matrix.
 * Author      : AI (with user input)
 * Modified    : June 2024
 * PURPOSE: Systematically isolate the faulty connection causing red lines, automatically repeating the sequence.
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA* dma_display = nullptr;
bool displayInitialized = false;
int testMode = 0; // Current test mode (0 for initial state, 1-7 for tests, 8 for recap)
unsigned long lastTestChange = 0;
const unsigned long TEST_INTERVAL_MS = 6000; // 6 seconds per test for observation
int cycleCount = 0; // Tracks how many full diagnostic cycles have completed
const int MAX_CYCLES = 10; // Number of times to repeat the full diagnostic sequence

// Define your S3 to HUB75 pin mapping again for clarity
// THESE MUST BE THE PINS YOU HAVE WIRED!
// Freenove WROOM ESP32-S3 on 64x64 HUB75
HUB75_I2S_CFG::i2s_pins current_pins = {
    .r1 = 42, .g1 = 41, .b1 = 40,
    .r2 = 38, .g2 = 39, .b2 = 37,
    .a = 45, .b = 48, .c = 47, .d = 21, .e = 14, // E pin (GPIO14) is CRITICAL for 64x64
    .lat = 2, .oe = 1, .clk = 36
};

// --- Helper Print Functions for Clear Serial Output ---
void printHeader(String title) {
  Serial.println("\n=======================================================");
  Serial.println(">>> " + title);
  Serial.println("=======================================================");
}

void printInstruction(String instruction) {
  Serial.println("   --> " + instruction);
}

void printObservation(String observation) {
  Serial.println("   OBSERVE: " + observation);
}

void printAction(String action) {
  Serial.println("   ACTION: " + action);
}

void setup() {
  Serial.begin(115200);
  delay(2000); // Give time for serial monitor to connect

  printHeader("Starting Auto-Pilot Deep Dive Matrix Diagnostics");
  printInstruction("This script will run " + String(MAX_CYCLES) + " full diagnostic cycles.");
  printInstruction("Ensure ALL wires are securely connected on both ends.");
  printInstruction("Especially check power (5V, GND) and your 'E' pin (GPIO14).");
  printInstruction("Power supply for matrix must be robust (e.g., 5V 4A or 5A).");
  printInstruction("The script will advance automatically every " + String(TEST_INTERVAL_MS/1000) + " seconds.");

  // Initialize display with standard config, no fancy retries (assume pins are correct)
  HUB75_I2S_CFG mxconfig(64, 64, 1, current_pins);
  mxconfig.double_buff = false; // Simpler for debugging
  mxconfig.clkphase = false;    // Default
  mxconfig.driver = HUB75_I2S_CFG::SHIFTREG; // Most common driver, avoids specific IC issues initially
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M; // Good balance

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  Serial.print("\nAttempting to initialize display... ");
  if (dma_display->begin()) {
    Serial.println("SUCCESS!");
    dma_display->setBrightness8(60); // Moderate brightness
    dma_display->clearScreen();
    displayInitialized = true;
    printInstruction("Display hardware initialized. Now we begin the automated tests.");
  } else {
    Serial.println("FAILED!");
    printInstruction("ERROR: Display initialization failed. This indicates a very fundamental problem.");
    printAction("1. IMMEDIATELY check 5V power supply to the matrix. Is it plugged in?");
    printAction("2. Check OE pin (GPIO1) and CLK pin (GPIO36). If these are dead, init can fail.");
    printAction("3. Check for shorts between matrix pins or to ground/5V.");
    Serial.println("Program halted. Fix issue and re-upload.");
    while (true) delay(1000); // Halt if initialization fails
  }

  // Initial power and clear screen test (before main loop starts)
  printHeader("Initial Check: Power and OE (Output Enable) - Cycle " + String(cycleCount + 1));
  printObservation("Matrix should be ENTIRELY BLACK/OFF. No lights whatsoever.");
  printAction("If you see *anything* (even faint red lines), check the OE pin (GPIO1) again.");
  dma_display->setBrightness8(0); // Set brightness to 0 (effectively off)
  dma_display->fillScreen(0);
  delay(TEST_INTERVAL_MS);

  dma_display->setBrightness8(60); // Restore brightness for upcoming tests
}

void loop() {
  if (!displayInitialized) return; // Should not happen if init failed in setup

  if (cycleCount >= MAX_CYCLES) {
    printHeader("+++ ALL " + String(MAX_CYCLES) + " DIAGNOSTIC CYCLES COMPLETED +++");
    printInstruction("The auto-pilot test sequence has finished.");
    printInstruction("Review the serial monitor output and your observations of the display.");
    printInstruction("If red lines or other issues persist, carefully re-check wiring based on specific test failures.");
    Serial.println("Program halted.");
    while(true) delay(1000); // Stop execution after all cycles are done
  }

  // Advance test mode if enough time has passed
  if (millis() - lastTestChange >= TEST_INTERVAL_MS) {
    testMode++;
    lastTestChange = millis();
    dma_display->clearScreen(); // Clear screen for each new test

    // If we've completed all tests (testMode 8 is the Recap/End-of-Cycle)
    if (testMode > 8) {
        cycleCount++;
        if (cycleCount < MAX_CYCLES) {
            printHeader("--- END OF CYCLE " + String(cycleCount) + " --- Starting Cycle " + String(cycleCount + 1) + " ---");
            testMode = 1; // Reset to the first test for the next cycle
        } else {
            // This will be caught by the cycleCount check at the beginning of loop()
            return; 
        }
    }

    // Execute the current test based on testMode
    switch (testMode) {
      case 1: // Test 1: Only RED (should already be working based on "red lines")
        printHeader("TEST 1: RED Screen Check (G1/G2/B1/B2 disconnection test) - Cycle " + String(cycleCount + 1));
        dma_display->fillScreen(dma_display->color565(255, 0, 0)); // FULL RED
        printObservation("Matrix should be COMPLETELY RED, no black lines, no other colors.");
        printAction("If you still see black or dotted lines, your address lines (A,B,C,D,E) even for RED are problematic, or CLK/LAT/OE are bad.");
        printAction("If it's RED but not full screen, your A,B,C,D,E address lines are faulty.");
        Serial.println("   This confirms R1/R2 and basic control lines are working.");
        break;

      case 2: // Test 2: Only GREEN (Key diagnostic for G1/G2)
        printHeader("TEST 2: GREEN Screen Check (Primary G1/G2 test) - Cycle " + String(cycleCount + 1));
        dma_display->fillScreen(dma_display->color565(0, 255, 0)); // FULL GREEN
        printObservation("Matrix should be COMPLETELY GREEN. NO RED AT ALL, no black lines.");
        printAction("If it remains RED OR BLACK: Your G1 (GPIO41) and/or G2 (GPIO39) data line wires are BAD or connected to the WRONG ESP32 pins.");
        printAction("   - Check G1/GPIO41 and G2/GPIO39 wiring.");
        printAction("   - Try swapping jumper wires for G1 and G2.");
        break;

      case 3: // Test 3: Only BLUE (Key diagnostic for B1/B2)
        printHeader("TEST 3: BLUE Screen Check (Primary B1/B2 test) - Cycle " + String(cycleCount + 1));
        dma_display->fillScreen(dma_display->color565(0, 0, 255)); // FULL BLUE
        printObservation("Matrix should be COMPLETELY BLUE. NO RED OR GREEN AT ALL, no black lines.");
        printAction("If it remains RED/GREEN OR BLACK: Your B1 (GPIO40) and/or B2 (GPIO37) data line wires are BAD or connected to the WRONG ESP32 pins.");
        printAction("   - Check B1/GPIO40 and B2/GPIO37 wiring.");
        printAction("   - Try swapping jumper wires for B1 and B2.");
        break;

      case 4: // Test 4: Top HALF White (Tests A,B,C,D address lines)
        printHeader("TEST 4: Top 32 Rows WHITE (Testing A,B,C,D) - E excluded - Cycle " + String(cycleCount + 1));
        dma_display->fillScreen(0); // Start black
        // Draw a white rectangle that fills the top half (0-31 rows)
        dma_display->fillRect(0, 0, 64, 32, dma_display->color565(255, 255, 255));
        printObservation("ONLY THE TOP HALF (rows 0-31) of the matrix should be entirely WHITE.");
        printObservation("The BOTTOM HALF (rows 32-63) should be entirely BLACK.");
        printAction("If top half is NOT white, or bottom half is NOT black:");
        printAction("   - This indicates a problem with A, B, C, D address lines (GPIO45, 48, 47, 21)");
        printAction("   - Also double check CLK (GPIO36), LAT (GPIO2), OE (GPIO1).");
        break;

      case 5: // Test 5: Bottom HALF White (Explicitly tests E address line)
        printHeader("TEST 5: Bottom 32 Rows WHITE (Explicitly Testing E) - Cycle " + String(cycleCount + 1));
        dma_display->fillScreen(0);
        // Draw a white rectangle that fills the bottom half (32-63 rows)
        dma_display->fillRect(0, 32, 64, 32, dma_display->color565(255, 255, 255));
        printObservation("ONLY THE BOTTOM HALF (rows 32-63) of the matrix should be entirely WHITE.");
        printObservation("The TOP HALF (rows 0-31) should be entirely BLACK.");
        printAction("If bottom half is NOT white (e.g., black, red lines, top half showing):");
        printAction("   - This is THE DEDICATED TEST for your E pin (GPIO14) or its connection.");
        printAction("   - YOUR E PIN (GPIO14) CABLE IS ALMOST CERTAINLY FAULTY, LOOSE, OR ON THE WRONG PIN.");
        printAction("   - SWAP OUT THE JUMPER WIRE FOR GPIO14 (E PIN) IMMEDIATELY.");
        break;

      case 6: // Test 6: Moving White Dot (Full panel scan and data integrity)
        printHeader("TEST 6: Moving White Dot (Animation Check - Full Panel) - Cycle " + String(cycleCount + 1));
        printInstruction("This test will loop. Watch closely for " + String(TEST_INTERVAL_MS/1000) + "s.");
        // The actual animation is handled in the continuous part of loop()
        break;

      case 7: // Test 7: Brightness test (Confirm control)
        printHeader("TEST 7: Brightness Fade (Final control check) - Cycle " + String(cycleCount + 1));
        printInstruction("This test will loop. Watch closely for " + String(TEST_INTERVAL_MS/1000) + "s.");
        // The actual animation is handled in the continuous part of loop()
        break;

      case 8: // Recap
        printHeader("DIAGNOSTIC RECAP AND NEXT STEPS - Cycle " + String(cycleCount + 1));
        Serial.println("Based on the tests, identify which specific screen was NOT as expected.");
        printAction("1. If Tests 2/3 (Green/Blue) failed: Focus on G1/G2/B1/B2 data lines (GPIO41, 39, 40, 37).");
        printAction("2. If Test 5 (Bottom Half White) failed: The E pin (GPIO14) is EXTREMELY likely the issue.");
        printAction("3. If Test 4 (Top Half White) failed: Issues with A,B,C,D (GPIO45, 48, 47, 21) or basic control (CLK, LAT, OE).");
        printAction("4. Re-check power if the brightness or overall display is dim/flickering, even if some colors appear.");
        printAction("5. IMPORTANT: USE A MULTIMETER FOR CONTINUITY!!! Check each wire from the ESP32 pin to the corresponding matrix header pin. This is often the real fix.");
        break;
    }
  }

  // --- Continuous Animations for specific test modes ---
  // These run constantly within their designated testMode duration
  if (testMode == 6) { // Moving White Dot
    static int pixelX = 0;
    static int pixelY = 0;
    static unsigned long lastMove = 0;
    if (millis() - lastMove > 50) { // Move every 50ms
      dma_display->clearScreen(); // Clear previous dot
      dma_display->drawPixel(pixelX, pixelY, dma_display->color565(255, 255, 255));
      pixelX++;
      if (pixelX >= 64) {
        pixelX = 0;
        pixelY++;
        if (pixelY >= 64) {
          pixelY = 0;
        }
      }
      lastMove = millis();
    }
  }

  if (testMode == 7) { // Brightness Fade
    static int brightness = 10;
    static int direction = 1;
    static unsigned long lastFade = 0;
    if (millis() - lastFade > 50) { // Change brightness every 50ms
      // Not clearing screen here to show persistent fade
      dma_display->fillScreenRGB888(0, 255, 0); // Green screen to show fade
      dma_display->setBrightness8(brightness);
      brightness += direction * 5;
      if (brightness >= 100) direction = -1;
      if (brightness <= 10) direction = 1;
      lastFade = millis();
    }
    // Added a small delay to avoid continuous re-rendering too fast
    delay(5); 
  }
  // --- End of Continuous Animations ---

  delay(1); // Small delay to yield to other tasks and not hog CPU
}



