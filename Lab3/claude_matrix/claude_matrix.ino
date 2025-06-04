#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Configuration for 64x64 matrix display
HUB75_I2S_CFG config;

void configureMatrix() {
  // Set panel dimensions
  config.mx_width = 64;   // Panel width
  config.mx_height = 64;  // Panel height
  config.chain_length = 1; // Number of panels chained
  
  // Configure GPIO pins - adjust these to match your wiring!
  config.gpio.r1 = 25;
  config.gpio.g1 = 26;
  config.gpio.b1 = 27;
  config.gpio.r2 = 14;
  config.gpio.g2 = 12;
  config.gpio.b2 = 13;
  config.gpio.a = 23;
  config.gpio.b = 22;
  config.gpio.c = 5;
  config.gpio.d = 17;
  config.gpio.e = 32;
  config.gpio.lat = 4;
  config.gpio.oe = 15;
  config.gpio.clk = 16;
}

// Display object pointer
MatrixPanel_I2S_DMA *dma_display = nullptr;

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial time to initialize
  Serial.println("Starting 64x64 Matrix Display...");

  // Configure the matrix settings
  configureMatrix();
  
  // Create display object
  dma_display = new MatrixPanel_I2S_DMA(config);

  // Allocate memory and start display
  if(!dma_display->begin()) {
    Serial.println("****** !KABOOM! I2S memory allocation failed ***********");
    for(;;) delay(500); // Stop execution
  }

  // Configure display settings
  dma_display->setBrightness8(90);    // 0-255 brightness
  dma_display->clearScreen();
  dma_display->fillScreen(0);         // Ensure screen is black

  // Test display with simple text
  dma_display->setCursor(2, 2);
  dma_display->setTextSize(1);
  dma_display->setTextColor(dma_display->color565(255, 0, 0)); // Red
  dma_display->print("64x64");
  
  dma_display->setCursor(2, 12);
  dma_display->setTextColor(dma_display->color565(0, 255, 0)); // Green  
  dma_display->print("Matrix");
  
  dma_display->setCursor(2, 22);
  dma_display->setTextColor(dma_display->color565(0, 0, 255)); // Blue
  dma_display->print("Display");

  // Draw some test pixels
  for(int i = 0; i < 64; i++) {
    dma_display->drawPixel(i, 32, dma_display->color565(255, 255, 255)); // White line
  }

  Serial.println("Display initialized successfully!");
}


// The new animation:

// Creates a smooth color transition from red → green → blue → red
// Uses dma_display->color565(r, g, b) to create 16-bit colors
// Cycles through 192 color steps for a smooth rainbow effect

// Available color methods in this library:

// color565(r, g, b) - Creates 16-bit color from 8-bit RGB values
// color444(r, g, b) - Creates 12-bit color from 4-bit RGB values
// color333(r, g, b) - Creates 9-bit color from 3-bit RGB values


void loop() {
  // Example animation: moving color bar
  static int offset = 0;
  static unsigned long lastUpdate = 0;
  
  if(millis() - lastUpdate > 100) { // Update every 100ms
    // Clear previous bar
    for(int x = 0; x < 64; x++) {
      dma_display->drawPixel(x, 40, 0);
    }
    
    // Draw new color bar with cycling colors
    for(int x = 0; x < 64; x++) {
      uint16_t color;
      int colorIndex = (x + offset) % 192; // Cycle through 192 color steps
      
      if(colorIndex < 64) {
        // Red to Green
        color = dma_display->color565(255 - (colorIndex * 4), colorIndex * 4, 0);
      } else if(colorIndex < 128) {
        // Green to Blue
        int step = colorIndex - 64;
        color = dma_display->color565(0, 255 - (step * 4), step * 4);
      } else {
        // Blue to Red
        int step = colorIndex - 128;
        color = dma_display->color565(step * 4, 0, 255 - (step * 4));
      }
      
      dma_display->drawPixel(x, 40, color);
    }
    
    offset = (offset + 1) % 192;
    lastUpdate = millis();
  }
  
  delay(10); // Small delay to prevent overwhelming the processor
}