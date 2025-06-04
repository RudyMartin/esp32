// What this code will do:

// Initialize the display with error checking
// Run 6 diagnostic tests in sequence
// Show different patterns in the main loop
// Print clear messages about what you should see

// Upload this and watch the serial monitor carefully. Tell me:

// Which tests show solid colors vs. the dotted line
// Do the colors match what the serial says should appear?
// Do you see the 5 individual pixels in test 6?
// Does the pattern change between the loop's checkerboard and horizontal lines?

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Configuration for 64x64 matrix display
HUB75_I2S_CFG config;

// Display object pointer
MatrixPanel_I2S_DMA *dma_display = nullptr;

void configureMatrix() {
  // Set panel dimensions
  config.mx_width = 64;   // Panel width
  config.mx_height = 64;  // Panel height
  config.chain_length = 1; // Number of panels chained
  config.i2sspeed = HUB75_I2S_CFG::HZ_10M; // Slower speed for stability
  
  // Configure GPIO pins - VERIFY these match your actual wiring!
  config.gpio.r1 = 25;   // Red upper half
  config.gpio.g1 = 26;   // Green upper half
  config.gpio.b1 = 27;   // Blue upper half
  config.gpio.r2 = 14;   // Red lower half
  config.gpio.g2 = 12;   // Green lower half
  config.gpio.b2 = 13;   // Blue lower half
  config.gpio.a = 23;    // Address line A
  config.gpio.b = 22;    // Address line B
  config.gpio.c = 5;     // Address line C
  config.gpio.d = 17;    // Address line D
  config.gpio.e = 33;    // Address line E (critical for 64-high panels)
  config.gpio.lat = 4;   // Latch pin
  config.gpio.oe = 15;   // Output enable (active low)
  config.gpio.clk = 16;  // Clock pin
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== 64x64 Matrix Display Test ===");

  // Configure the matrix settings
  configureMatrix();
  
  // Create display object
  dma_display = new MatrixPanel_I2S_DMA(config);

  // Initialize display
  if (!dma_display->begin()) {
    Serial.println("ERROR: Display initialization failed!");
    while(1) {
      delay(1000);
      Serial.println("Check connections and power supply");
    }
  }

  Serial.println("Display initialized successfully!");

  // Set brightness and clear screen
  dma_display->setBrightness8(80);  // Medium brightness
  dma_display->clearScreen();
  
  // Run diagnostic tests
  runDiagnosticTests();
}

void runDiagnosticTests() {
  Serial.println("\n=== Starting Diagnostic Tests ===");
  
  // Test 1: Clear screen (should be black)
  Serial.println("Test 1: Clear screen");
  dma_display->fillScreen(0);
  delay(2000);
  
  // Test 2: Fill with dim white
  Serial.println("Test 2: Dim white screen");
  dma_display->fillScreen(dma_display->color565(64, 64, 64));
  delay(3000);
  
  // Test 3: Red screen
  Serial.println("Test 3: Red screen");
  dma_display->fillScreen(dma_display->color565(255, 0, 0));
  delay(3000);
  
  // Test 4: Green screen
  Serial.println("Test 4: Green screen");
  dma_display->fillScreen(dma_display->color565(0, 255, 0));
  delay(3000);
  
  // Test 5: Blue screen
  Serial.println("Test 5: Blue screen");
  dma_display->fillScreen(dma_display->color565(0, 0, 255));
  delay(3000);
  
  // Test 6: Corner pixels
  Serial.println("Test 6: Corner and center pixels");
  dma_display->fillScreen(0);
  dma_display->drawPixel(0, 0, dma_display->color565(255, 255, 255));      // Top-left
  dma_display->drawPixel(63, 0, dma_display->color565(255, 0, 0));         // Top-right red
  dma_display->drawPixel(0, 63, dma_display->color565(0, 255, 0));         // Bottom-left green
  dma_display->drawPixel(63, 63, dma_display->color565(0, 0, 255));        // Bottom-right blue
  dma_display->drawPixel(32, 32, dma_display->color565(255, 255, 0));      // Center yellow
  
  Serial.println("\n=== Diagnostic Tests Complete ===");
  Serial.println("What do you see?");
  Serial.println("1. Solid color screens during tests 2-5?");
  Serial.println("2. 5 colored pixels in test 6?");
  Serial.println("3. Still just the dotted line?");
  Serial.println("4. Something else?");
}

void loop() {
  // Simple alternating pattern to help identify issues
  static bool toggle = false;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate > 2000) {  // Every 2 seconds
    if (toggle) {
      // Draw checkerboard pattern
      for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
          if ((x + y) % 2 == 0) {
            dma_display->drawPixel(x, y, dma_display->color565(100, 100, 100));
          } else {
            dma_display->drawPixel(x, y, 0);
          }
        }
      }
      Serial.println("Checkerboard pattern");
    } else {
      // Draw horizontal lines
      dma_display->fillScreen(0);
      for (int y = 0; y < 64; y += 4) {
        for (int x = 0; x < 64; x++) {
          dma_display->drawPixel(x, y, dma_display->color565(150, 150, 150));
        }
      }
      Serial.println("Horizontal lines pattern");
    }
    
    toggle = !toggle;
    lastUpdate = millis();
  }
  
  delay(10);
}