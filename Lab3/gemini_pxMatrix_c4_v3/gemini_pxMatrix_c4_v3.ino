/**********************************************************************
 * Filename    : PxMatrix_Deep_Dive_RedLines_AutoPilot.ino
 * Description : PxMatrix-based diagnostic for persistent red lines / "just lines" on 64x64 matrix.
 * Author      : AI (adapted for PxMatrix)
 * Modified    : June 2024
 * PURPOSE: Systematically isolate the faulty connection causing "just lines" using PxMatrix.
 **********************************************************************/

#include "PxMatrix.h"

#ifdef ESP32

// --- PxMatrix Pin Definitions for Freenove ESP32-S3 ---
// These are the SAFE pins identified for the ESP32-S3, replacing the default PxMatrix ESP32 pins.
// Make sure to connect your physical wires according to these new pin numbers!

// Data Pins (R1, G1, B1, R2, G2, B2)
// These are typically picked up by the library's internal I2S/RMT peripheral setup.
// Your previously working pins for these are already good general-purpose GPIOs on S3.


hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif // ESP32

#ifdef ESP8266
// Keep this section as-is if you also support ESP8266, but it won't be used for your ESP32-S3
#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2
#endif // ESP8266

// Pins for LED MATRIX
#define matrix_width 64
#define matrix_height 64

// This defines the 'on' time of the display in us. The larger this number,
// the brighter the display. If too large the ESP will crash
uint8_t display_draw_time = 10; //10-50 is usually fine

// PxMATRIX display MUST be declared AFTER all the 'P_' defines it relies on.
PxMATRIX display(matrix_width, matrix_height, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);

uint16_t myCOLORS[8] = {myRED, myGREEN, myBLUE, myWHITE, myYELLOW, myCYAN, myMAGENTA, myBLACK};

// Converted using the following site: http://www.rinkydinkelectronics.com/t_imageconverter565.php
// ... mario[] array (keep as is)
uint16_t static mario[] = {
  // ... your very long mario array goes here ...
  0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, // 0x0010 (16) pixels
  // ... (TRUNCATED FOR BREVITY IN THIS EXAMPLE) ...
  0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, 0x4FE0, // 0x0800 (2048) pixels
};


#ifdef ESP8266
// ISR for display refresh
void display_updater() {
  display.display(display_draw_time);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater() {
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(display_draw_time);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void display_update_enable(bool is_enable) {
  if (is_enable) {
    timer = timerBegin(0); // Only pass the timer index (0, 1, 2, or 3)
    timerSetDivider(timer, 80); // Set the prescaler separately
    timerSetCountMode(timer, TIMER_COUNT_UP); // Set count direction (TIMER_COUNT_UP or TIMER_COUNT_DOWN)

    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 50, true); // Alarm every 50 microseconds (for 50us refresh)
    timerAlarmEnable(timer);
  } else {
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }
}
#endif // ESP32


void setup() {
  Serial.begin(115200); // Changed to 115200 for faster debug output
  Serial.println("Starting PxMatrix setup...");

  display.begin(32); // For 64x64 matrix, 1/32 scan rate is common
  display.setPanelType(HUB75_64x64_MOD32SCAN); // Or your specific panel type
  display.setScanPattern(LINE_INTERLEAVE_SCAN_PATTERN); // Common for 64x64
  display.setMuxPattern(BIT_ADDRESSED_MUX_PATTERN); // Common for modern panels
  display.setDriverChip(FM6126A); // Common driver chip
  display.setBrightness(100); // Adjust as needed

  display.clearDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2, 0);
  display.print("Pixel");
  display.setTextColor(myMAGENTA);
  display.setCursor(2, 8);
  display.print("Time");
  display_update_enable(true); // Start the refresh ISR

  display.display(); // Push the initial "Pixel Time" to matrix

  Serial.println("PxMatrix setup complete. Initial text displayed.");
  delay(3000);
}

void drawImage(int x, int y, uint16_t color) {
  int imageHeight = 33;
  int imageWidth = 33;
  for (int yy = 0; yy < imageHeight; yy++) {
    for (int xx = 0; xx < imageWidth; xx++) {
      if (xx + x >= 0 && xx + x < matrix_width && yy + y >= 0 && yy + y < matrix_height) { // Boundary check
        display.drawPixel(xx + x, yy + y, color);
      }
    }
  }
}

void drawImage2(int x, int y) {
  int imageHeight = 32; // Assuming mario[] is 64x32
  int imageWidth = 64;
  int counter = 0;
  for (int yy = 0; yy < imageHeight; yy++) {
    for (int xx = 0; xx < imageWidth; xx++) {
      // Corrected boundary check for mario array + display
      if (counter < sizeof(mario) / sizeof(mario[0])) { // Prevent reading past array end
        if (xx + x >= 0 && xx + x < matrix_width && yy + y >= 0 && yy + y < matrix_height) { // Boundary check for display
          display.drawPixel(xx + x, yy + y, mario[counter]);
        }
      }
      counter++;
    }
  }
}

int cnt = 0;
void loop() {
  cnt++;
  delay(1000); // 1 second delay between updates
  display.clearDisplay(); // Clear the display for the next frame

  if (cnt == 1) {
    drawImage(matrix_width / 2 - 16, matrix_height / 2 - 16, myWHITE); // Center a 33x33 square
    Serial.println("Test White Square");
  } else if (cnt == 2) {
    drawImage2(0, 8); // Draw Mario image (from row 8)
    drawImage(1, 1, myRED); // Draw a red square on top
    Serial.println("Test Mario + Red Square");
  } else if (cnt == 3) {
    drawImage(matrix_width / 2 - 16, matrix_height / 2 - 16, myGREEN); // Center a 33x33 square
    Serial.println("Test Green Square");
  } else if (cnt == 4) {
    drawImage2(0, 32); // Draw Mario image (from row 32)
    drawImage(1, 1, myBLUE); // Draw a blue square on top
    Serial.println("Test Mario + Blue Square");
  } else if (cnt == 5) {
    drawImage2(0, 0); // Draw Mario image (from row 0)
    Serial.println("Test Mario from row 0");
  } else if (cnt == 0 || cnt == 6) { // Loop ends after 6 tests (0-5 or 1-6)
    drawImage2(0, 16); // Draw Mario image (from row 16)
    Serial.println("Test Mario from row 16");
    cnt = 0; // Reset counter
  }
  // The display.display() call is handled by the ISR.
}