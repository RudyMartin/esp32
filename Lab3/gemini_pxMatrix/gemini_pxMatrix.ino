/**********************************************************************
 * Filename    : PxMatrix_Deep_Dive_RedLines_AutoPilot.ino
 * Description : PxMatrix-based diagnostic for persistent red lines / "just lines" on 64x64 matrix.
 * Author      : AI (adapted for PxMatrix)
 * Modified    : June 2024
 * PURPOSE: Systematically isolate the faulty connection causing "just lines" using PxMatrix.
 **********************************************************************/

// Include the PxMatrix library
#include <PxMatrix.h>

// --- PxMatrix Pin Definitions (Based on previous Option 1a - Recommended Safe Pins) ---
// Remember to connect these to the corresponding pins on your 16-pin HUB75 connector.

// RGB Data Pins (often hardcoded within the PxMatrix library or defined here)
#define P_R1  42 // Pin 1 on matrix cable
#define P_G1  41 // Pin 2 on matrix cable
#define P_B1  40 // Pin 3 on matrix cable
#define P_R2  38 // Pin 4 on matrix cable
#define P_G2  39 // Pin 5 on matrix cable
#define P_B2  37 // Pin 6 on matrix cable

// Address Pins
#define P_A   45 // Pin 7 on matrix cable
#define P_B   48 // Pin 8 on matrix cable
#define P_C   47 // Pin 9 on matrix cable
#define P_D   15 // Pin 10 on matrix cable (Changed from 21: avoiding sensitive flash pin)
#define P_E   16 // Pin 11 on matrix cable (Changed from 14: avoiding sensitive JTAG pin)

// Control Pins
#define P_LAT 17 // Pin 13 on matrix cable (Changed from 2: avoiding UART TX)
#define P_OE  18 // Pin 14 on matrix cable (Changed from 1: avoiding UART RX)
#define P_CLK 19 // Pin 15 on matrix cable (Changed from 36: using a general purpose GPIO)

// --- Matrix Dimensions ---
const int matrix_width = 64;
const int matrix_height = 64;

// Panel type for a 64x64 matrix:
// For 64x64, it's typically a 1/32 scan rate (meaning 32 lines multiplexed, so E is used).
// You might need to adjust this depending on the exact panel.
// Common options for 64x64 are MOD32SCAN (5 address lines A,B,C,D,E)
// If it was 64x32, it would often be MOD16SCAN (4 address lines A,B,C,D)
const PxMatrix::paneltype_t panel_type = HUB75_64x64_MOD32SCAN; // OR HUB75_64x64_MOD16SCAN if your matrix is different

// Initialize PxMatrix object with control/address pins
// NOTE: Make sure the defines for P_R1...P_B2 and P_CLK are done *before* this line (or at least before PxMatrix.h is included if it uses them internally for setup).
// The order here *should* be fine if PxMatrix library's internal code picks up the defines correctly.
PxMATRIX display(matrix_width, matrix_height, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting PxMatrix setup...");

  // Initialize the display
  display.begin();

  // Set the panel type
  display.setPanelType(panel_type);

  // Set scan pattern and mux pattern (often needed for 64x64 displays)
  // LINE_INTERLEAVE_SCAN_PATTERN is common for 64x64. You might also try ZAGGED_SCAN_PATTERN.
  display.setScanPattern(LINE_INTERLEAVE_SCAN_PATTERN);
  display.setMuxPattern(BIT_ADDRESSED_MUX_PATTERN); // This is common for modern HUB75 panels

  // Set brightness (0-255)
  display.setBrightness(100);

  // Clear display initially
  display.clearDisplay();
  display.display(); // Push the cleared buffer to the matrix
  Serial.println("PxMatrix setup complete. Displaying test patterns.");
}

void loop() {
  // Test 1: Simple text
  display.clearDisplay();
  display.setTextColor(display.COLOR_WHITE);
  display.setCursor(0, 0);
  display.print("Hello");
  display.setCursor(0, 10);
  display.print("ESP32-S3!");
  display.display(); // Show the buffer
  delay(2000);

  // Test 2: Fill with Red
  display.clearDisplay();
  display.fillScreen(display.COLOR_RED);
  display.display();
  delay(1000);

  // Test 3: Fill with Green
  display.clearDisplay();
  display.fillScreen(display.COLOR_GREEN);
  display.display();
  delay(1000);

  // Test 4: Fill with Blue
  display.clearDisplay();
  display.fillScreen(display.COLOR_BLUE);
  display.display();
  delay(1000);

  // Test 5: Alternating pixels
  display.clearDisplay();
  for (int x = 0; x < matrix_width; x++) {
    for (int y = 0; y < matrix_height; y++) {
      if ((x + y) % 2 == 0) {
        display.drawPixel(x, y, display.COLOR_WHITE);
      } else {
        display.drawPixel(x, y, display.COLOR_BLACK);
      }
    }
  }
  display.display();
  delay(2000);
}