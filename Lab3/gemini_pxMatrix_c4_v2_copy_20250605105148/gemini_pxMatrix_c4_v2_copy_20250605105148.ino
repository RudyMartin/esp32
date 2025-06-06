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
#define P_R1  42 // Red 1  - Pin 1 on matrix cable
#define P_G1  41 // Green 1 - Pin 2 on matrix cable
#define P_B1  40 // Blue 1 - Pin 3 on matrix cable
#define P_R2  38 // Red 2  - Pin 4 on matrix cable
#define P_G2  39 // Green 2 - Pin 5 on matrix cable
#define P_B2  37 // Blue 2 - Pin 6 on matrix cable

// Control and Address Pins (these are passed into the PxMATRIX constructor)
// Changed from common ESP32 pins (22, 16, 5, 15) to S3-safe ones.
#define P_LAT 17 // Latch           - Pin 13 on matrix cable (Changed from 22, avoids UART TX on S3)
#define P_OE  18 // Output Enable   - Pin 14 on matrix cable (Changed from 16, avoids UART RX on S3)
#define P_A   45 // Address A       - Pin 7 on matrix cable
#define P_B   48 // Address B       - Pin 8 on matrix cable
#define P_C   47 // Address C       - Pin 9 on matrix cable
#define P_D   15 // Address D       - Pin 10 on matrix cable (Changed from 5, sensitive ESP32 pin on S3)
#define P_E   16 // Address E       - Pin 11 on matrix cable (Changed from 15, sensitive ESP32 pin on S3)

// Clock Pin (also typically picked up by the library for internal setup)
// Changed from 36 (a strapping pin) for better practice, though it might work.
#define P_CLK 19 // Clock           - Pin 15 on matrix cable (Using good general-purpose GPIO)


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
#endif


void display_update_enable(bool is_enable) {

  if (is_enable) {
    // --- START MODIFICATION HERE ---
    // Old: timer = timerBegin(0, 80, true);
    timer = timerBegin(0); // Only pass the timer index (0, 1, 2, or 3)
    timerSetDivider(timer, 80); // Set the prescaler separately
    timerSetCountMode(timer, TIMER_COUNT_UP); // Set count direction (TIMER_COUNT_UP or TIMER_COUNT_DOWN)
    // --- END MODIFICATION HERE ---

    timerAttachInterrupt(timer, &display_updater, true);
    // timerAlarmWrite takes timer pointer, alarm value, and auto_reload (true/false)
    // 4000 * 12.5ns = 50us (for 80MHz clock, 80 divider means 1MHz timer clock, so 4000 ticks = 4ms)
    // Wait, 4000 ticks / 1MHz = 4ms. That's a 4ms refresh. For 64x64, you typically want faster, like 50us.
    // Let's re-evaluate the freq_div and alarm_value.
    // If you want 50us refresh, the period is 50,000 ns.
    // Timer clock after divider: 80MHz (CPU) / 80 (divider) = 1MHz.
    // So 1 tick = 1us.
    // For 50us refresh, alarm value should be 50.
    timerAlarmWrite(timer, 50, true); // Changed from 4000 to 50 for a faster 50µs refresh rate.
    timerAlarmEnable(timer);
  } else {
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }

#endif // ESP32
}


void setup() {
  Serial.begin(115200); // Changed to 115200 for faster debug output
  Serial.println("Starting PxMatrix setup...");

  // Define your display layout here, e.g. 1/8 step
  // For 64x64 matrix, 32 multiplexes (1/32 scan) is common and expected with 5 address lines (A-E)
  display.begin(32); // This sets the scan rate/number of multiplexed lines

  // Set the panel type based on your matrix specifications
  // PxMatrix has predefined panel types common for HUB75.
  // For 64x64 with 5 address lines, HUB75_64x64_MOD32SCAN is most likely.
  // If your matrix is 1/16 scan (e.g., two 64x32 panels stacked), it would be HUB75_64x64_MOD16SCAN.
  display.setPanelType(HUB75_64x64_MOD32SCAN);

  // Define scan pattern here {LINE, ZIGZAG, ZAGGIZ, WZAGZIG, VZAG} (default is LINE)
  // For 64x64, LINE_INTERLEAVE_SCAN_PATTERN is common. Use specific enum value.
  display.setScanPattern(LINE_INTERLEAVE_SCAN_PATTERN);

  // Define multiplex implementation here {BINARY, STRAIGHT} (default is BINARY)
  // BIT_ADDRESSED_MUX_PATTERN is common for modern panels.
  display.setMuxPattern(BIT_ADDRESSED_MUX_PATTERN); // Or keep default BINARY if it works

  // Set driver chip based on your matrix. FM6126A is very common.
  display.setDriverChip(FM6126A);

  // Set brightness (0-255)
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
  // This function currently draws a solid square, not an image.
  // The 'color' argument seems to be used correctly here.
  for (int yy = 0; yy < imageHeight; yy++) {
    for (int xx = 0; xx < imageWidth; xx++) {
      if (xx + x < matrix_width && yy + y < matrix_height) { // Boundary check
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
      if (counter < sizeof(mario) / sizeof(mario[0])) { // Prevent reading past array end
        if (xx + x < matrix_width && yy + y < matrix_height) { // Boundary check
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
    drawImage(1, 1, myWHITE); // Draw a white square
    Serial.println("Test White Square");
  } else if (cnt == 2) {
    drawImage2(0, 8); // Draw Mario image (from row 8)
    drawImage(1, 1, myRED); // Draw a red square on top
    Serial.println("Test Mario + Red Square");
  } else if (cnt == 3) {
    drawImage(1, 1, myGREEN); // Draw a green square
    Serial.println("Test Green Square");
  } else if (cnt == 4) {
    drawImage2(0, 32); // Draw Mario image (from row 32)
    drawImage(1, 1, myBLUE); // Draw a blue square on top
    Serial.println("Test Mario + Blue Square");
    cnt = 0; // Reset counter after this
  } else if (cnt == 5) { // Assuming you want 6 states before looping (0-5 or 1-6)
    drawImage2(0, 0); // Draw Mario image (from row 0)
    Serial.println("Test Mario from row 0");
  } else if (cnt == 6) {
    drawImage2(0, 16); // Draw Mario image (from row 16)
    Serial.println("Test Mario from row 16");
    cnt = 0; // Reset counter
  }

  // Important: After drawing to the buffer, you need to tell PxMatrix to push it to the display.
  // The `display.display()` call is handled by the ISR `display_updater()`, which is called automatically
  // by `display_update_enable(true)` in setup(). So you don't call it here directly in loop().
  // The ISR continuously refreshes the display from the current buffer.
}