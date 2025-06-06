/**********************************************************************
 * Filename    : PxMatrix_Deep_Dive_RedLines_AutoPilot.ino
 * Description : PxMatrix-based diagnostic for persistent red lines / "just lines" on 64x64 matrix.
 * Author      : AI (adapted for PxMatrix)
 * Modified    : June 2024
 * PURPOSE: Systematically isolate the faulty connection causing "just lines" using PxMatrix.
 **********************************************************************/

#include <PxMatrix.h>
#include <Ticker.h> // PxMatrix often uses Ticker for display updates

// --- Pin Definitions ---
// THESE MUST BE THE PINS YOU HAVE WIRED!
// Freenove WROOM ESP32-S3 on 64x64 HUB75
// Make sure these match getPinByName's mapping if you are using that in config
// PxMatrix constructor needs direct pin numbers.
#define P_R1 42
#define P_G1 41
#define P_B1 40
#define P_R2 38
#define P_G2 39
#define P_B2 37
#define P_A  45
#define P_B  48
#define P_C  47
#define P_D  21
#define P_E  14 // E pin (GPIO14) is CRITICAL for 64x64
#define P_LAT 2
#define P_OE  1
#define P_CLK 36

// --- Display Configuration ---
const int MATRIX_WIDTH = 64;
const int MATRIX_HEIGHT = 64;
const int PANEL_SCAN_RATE = 32; // For 64-height, it's 1/32 scan.

PxMATRIX* display = nullptr;
Ticker display_ticker;

// Function to update the display (PxMatrix requires this)
void display_updater() {
  display->display(70); // The 70 is a refresh rate parameter for PxMatrix
}

// --- Diagnostic State Variables ---
int testMode = 0; // Current test mode (0 for initial state, 1-7 for tests, 8 for recap)
unsigned long lastTestChange = 0;
const unsigned long TEST_INTERVAL_MS = 6000; // 6 seconds per test for observation
int cycleCount = 0; // Tracks how many full diagnostic cycles have completed
const int MAX_CYCLES = 10; // Number of times to repeat the full diagnostic sequence

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
  delay(20000); // Give time for serial monitor to connect

  printHeader("Starting PxMatrix-based Deep Dive Matrix Diagnostics");
  printInstruction("This script will run " + String(MAX_CYCLES) + " full diagnostic cycles.");
  printInstruction("Ensure ALL wires are securely connected on both ends, especially E (GPIO" + String(P_E) + ").");
  printInstruction("Power supply for matrix must be robust (e.g., 5V 4A or 5A).");
  printInstruction("The script will advance automatically every " + String(TEST_INTERVAL_MS/1000) + " seconds.");

  // Initialize PxMatrix
  // PxMatrix(width, height, LATCH, OE, CLK, R1, G1, B1, R2, G2, B2, A, B, C, D, E)
  // For 1/32 Scan (64x64), the constructor signature is:
  // PxMATRIX(width, height, A, B, C, D, E, LAT, OE, CLK) -- simpler, as data pins are implied by I2S setup
  // BUT the common PxMatrix constructor for ESP32 often takes pins in this order:
  // PxMATRIX(width, height, LATCH, OE, CLK, A, B, C, D, E) -- for Address lines first
  // Let's use the explicit ESP32 variant of PxMatrix which lists all pins.
  // PxMatrix(width, height, LATCH, OE, CLK, R1, G1, B1, R2, G2, B2, A, B, C, D, E); (This is for ESP32 direct constructor)

  display = new PxMATRIX(MATRIX_WIDTH, MATRIX_HEIGHT, P_LAT, P_OE, P_CLK, P_R1, P_G1, P_B1, P_R2, P_G2, P_B2, P_A, P_B, P_C, P_D, P_E);

  // You can also try other constructors depending on your PxMatrix version or specific needs:
  // For example, if you explicitly configure I2S pins yourself or use a different adapter board.
  // PxMATRIX display(64, 64, P_LAT, P_OE, P_CLK, P_A, P_B, P_C, P_D, P_E); // This is for 1/32 panels, but assumes data pins are preset/fixed.
  // The first `PxMATRIX(..., R1..B2, A..E)` is more robust for general ESP32 use.

  display->begin(PANEL_SCAN_RATE); // Specify 1/32 scan rate
  
  // Set brightness and clear screen
  display->setBrightness(60); // 0-255 range for PxMatrix brightness
  display->clearDisplay();    // PxMatrix clear function
  
  // Attach ticker for continuous display updates
  display_ticker.attach_ms(1, display_updater); // PxMatrix needs frequent updates

  printInstruction("Display hardware initialized via PxMatrix. Now we begin the automated tests.");

  // Initial power and clear screen test (before main loop starts)
  printHeader("Initial Check: Power and OE (Output Enable) - Cycle " + String(cycleCount + 1));
  printObservation("Matrix should be ENTIRELY BLACK/OFF. No lights whatsoever.");
  printAction("If you see *anything* (even faint lines), check the OE pin (GPIO" + String(P_OE) + ") again.");
  display->setBrightness(0); // Set brightness to 0 (effectively off)
  display->clearDisplay();   // Clear buffer
  display->showBuffer();     // Push empty buffer
  delay(TEST_INTERVAL_MS);

  display->setBrightness(60); // Restore brightness for upcoming tests
}

void loop() {
  if (cycleCount >= MAX_CYCLES) {
    printHeader("+++ ALL " + String(MAX_CYCLES) + " DIAGNOSTIC CYCLES COMPLETED +++");
    printInstruction("The auto-pilot test sequence has finished.");
    printInstruction("Review the serial monitor output and your observations of the display.");
    printInstruction("If lines or other issues persist, carefully re-check wiring based on specific test failures.");
    Serial.println("Program halted.");
    while(true) delay(10000); // Stop execution after all cycles are done
  }

  // Advance test mode if enough time has passed
  if (millis() - lastTestChange >= TEST_INTERVAL_MS) {
    testMode++;
    lastTestChange = millis();
    display->clearDisplay(); // Clear screen for each new test
    display->showBuffer();   // Push cleared buffer immediately

    // If we've completed all tests (testMode 8 is the Recap/End-of-Cycle)
    if (testMode > 8) {
        cycleCount++;
        if (cycleCount < MAX_CYCLES) {
            printHeader("--- END OF CYCLE " + String(cycleCount) + " --- Starting Cycle " + String(cycleCount + 1) + " ---");
            testMode = 1; // Reset to the first test for the next cycle
        } else {
            return; // Will be caught by the cycleCount check at the beginning of loop()
        }
    }

    // Execute the current test based on testMode
    switch (testMode) {
      case 1: // Test 1: Only RED (should already be working based on "red lines")
        printHeader("TEST 1: RED Screen Check - Cycle " + String(cycleCount + 1));
        display->fillScreen(display->color565(255, 0, 0)); // FULL RED
        printObservation("Matrix should be COMPLETELY RED, no black lines, no other colors.");
        printAction("If you still see black or fuzzy lines, your address lines (A,B,C,D,E) even for RED are problematic, or CLK/LAT/OE are bad.");
        printAction("If it's RED but not full screen, your A,B,C,D,E address lines are faulty.");
        break;

      case 2: // Test 2: Only GREEN (Key diagnostic for G1/G2)
        printHeader("TEST 2: GREEN Screen Check - Cycle " + String(cycleCount + 1));
        display->fillScreen(display->color565(0, 255, 0)); // FULL GREEN
        printObservation("Matrix should be COMPLETELY GREEN. NO RED AT ALL, no black lines.");
        printAction("If it remains RED OR BLACK or is incorrect color: Your G1 (GPIO" + String(P_G1) + ") and/or G2 (GPIO" + String(P_G2) + ") data line wires are BAD or connected to the WRONG ESP32 pins.");
        break;

      case 3: // Test 3: Only BLUE (Key diagnostic for B1/B2)
        printHeader("TEST 3: BLUE Screen Check - Cycle " + String(cycleCount + 1));
        display->fillScreen(display->color565(0, 0, 255)); // FULL BLUE
        printObservation("Matrix should be COMPLETELY BLUE. NO RED OR GREEN AT ALL, no black lines.");
        printAction("If it remains RED/GREEN OR BLACK or is incorrect color: Your B1 (GPIO" + String(P_B1) + ") and/or B2 (GPIO" + String(P_B2) + ") data line wires are BAD or connected to the WRONG ESP32 pins.");
        break;

      case 4: // Test 4: Top HALF White (Tests A,B,C,D address lines)
        printHeader("TEST 4: Top 32 Rows WHITE (Testing A,B,C,D) - E excluded - Cycle " + String(cycleCount + 1));
        display->fillScreen(0); // Start black
        display->fillRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT / 2, display->color565(255, 255, 255)); // Fill top half
        printObservation("ONLY THE TOP HALF (rows 0-31) of the matrix should be entirely WHITE.");
        printObservation("The BOTTOM HALF (rows 32-63) should be entirely BLACK.");
        printAction("If top half is NOT white or is garbled, or bottom half is NOT black:");
        printAction("   - This indicates a problem with A, B, C, D address lines (GPIO" + String(P_A) + ", " + String(P_B) + ", " + String(P_C) + ", " + String(P_D) + ").");
        break;

      case 5: // Test 5: Bottom HALF White (Explicitly tests E address line)
        printHeader("TEST 5: Bottom 32 Rows WHITE (Explicitly Testing E) - Cycle " + String(cycleCount + 1));
        display->fillScreen(0);
        display->fillRect(0, MATRIX_HEIGHT / 2, MATRIX_WIDTH, MATRIX_HEIGHT / 2, display->color565(255, 255, 255)); // Fill bottom half
        printObservation("ONLY THE BOTTOM HALF (rows 32-63) of the matrix should be entirely WHITE.");
        printObservation("The TOP HALF (rows 0-31) should be entirely BLACK.");
        printAction("If bottom half is NOT white (e.g., black, lines, top half showing):");
        printAction("   - This is THE DEDICATED TEST for your E pin (GPIO" + String(P_E) + ") or its connection.");
        printAction("   - YOUR E PIN (GPIO" + String(P_E) + ") CABLE IS ALMOST CERTAINLY FAULTY, LOOSE, OR ON THE WRONG PIN.");
        printAction("   - SWAP OUT THE JUMPER WIRE FOR GPIO" + String(P_E) + " (E PIN) IMMEDIATELY.");
        break;

      case 6: // Test 6: Moving White Dot (Full panel scan and data integrity)
        printHeader("TEST 6: Moving White Dot (Animation Check - Full Panel) - Cycle " + String(cycleCount + 1));
        printInstruction("This test will loop. Watch closely for " + String(TEST_INTERVAL_MS/1000) + "s.");
        break;

      case 7: // Test 7: Brightness test (Confirm control)
        printHeader("TEST 7: Brightness Fade (Final control check) - Cycle " + String(cycleCount + 1));
        printInstruction("This test will loop. Watch closely for " + String(TEST_INTERVAL_MS/1000) + "s.");
        break;

      case 8: // Recap
        printHeader("DIAGNOSTIC RECAP AND NEXT STEPS - Cycle " + String(cycleCount + 1));
        Serial.println("Based on the tests, identify which specific screen was NOT as expected.");
        printAction("1. If Tests 2/3 (Green/Blue) failed or were incorrect colors: Focus on G1/G2/B1/B2 data lines (GPIO" + String(P_G1) + ", " + String(P_G2) + ", " + String(P_B1) + ", " + String(P_B2) + ").");
        printAction("2. If Test 5 (Bottom Half White) failed: The E pin (GPIO" + String(P_E) + ") is EXTREMELY likely the issue.");
        printAction("3. If Test 4 (Top Half White) failed: Issues with A,B,C,D (GPIO" + String(P_A) + ", " + String(P_B) + ", " + String(P_C) + ", " + String(P_D) + ") or basic control (CLK, LAT, OE).");
        printAction("4. Re-check power if the brightness or overall display is dim/flickering, even if some colors appear.");
        printAction("5. IMPORTANT: USE A MULTIMETER FOR CONTINUITY. Check each wire from the ESP32 pin to the corresponding matrix header pin. This is often the real fix.");
        break;
    }
  }

  // --- Continuous Animations for specific test modes ---
  if (testMode == 6) { // Moving White Dot
    static int pixelX = 0;
    static int pixelY = 0;
    static unsigned long lastMove = 0;
    if (millis() - lastMove > 50) { // Move every 50ms
      display->clearDisplay(); // Clear previous dot
      display->drawPixel(pixelX, pixelY, display->color565(255, 255, 255));
      pixelX++;
      if (pixelX >= MATRIX_WIDTH) {
        pixelX = 0;
        pixelY++;
        if (pixelY >= MATRIX_HEIGHT) {
          pixelY = 0;
        }
      }
      lastMove = millis();
    }
  }

  if (testMode == 7) { // Brightness Fade
    static int brightness_val = 10; // Use a different variable name to avoid conflict with display->setBrightness()
    static int direction = 1;
    static unsigned long lastFade = 0;
    if (millis() - lastFade > 50) { // Change brightness every 50ms
      display->fillScreen(display->color565(0, 255, 0)); // Green screen to show fade
      display->setBrightness(brightness_val); // PxMatrix brightness 0-255
      brightness_val += direction * 5;
      if (brightness_val >= 250) direction = -1; // Max brightness near 255
      if (brightness_val <= 10) direction = 1;   // Min brightness
      lastFade = millis();
    }
  }
  // --- End of Continuous Animations ---

  // PxMatrix requires showBuffer() to push changes from buffer to display
  // This should happen regularly, driven by the Ticker, but ensure it's still called if main loop is fast
  // (In this setup, the Ticker's display_updater() handles it, so we don't need it here unless specific drawing is done outside that.)
  // display->showBuffer(); // (Only needed if you draw and want to force update without waiting for ticker)

  delay(1); // Small delay to yield to other tasks and not hog CPU
}
