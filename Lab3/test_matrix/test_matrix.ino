#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // Ensure this is included

// Correct type name for the configuration struct
HUB75_I2S_CFG config; 

// Parameters for your display:
const int panelResX = 64;   // Number of pixels wide of each INDIVIDUAL panel module.
const int panelResY = 64;   // Number of pixels high of each INDIVIDUAL panel module.
const int chainLength = 1;  // Number of LED panels chained together (e.g. 2 x 64x32 panels is chainLength = 2)

// Declare the display object pointer
MatrixPanel_I2S_DMA *dma_display = nullptr;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting...");

  // Populate the configuration struct with your panel details and GPIO pins
  config.mx_width = panelResX;
  config.mx_height = panelResY;
  config.chain_length = chainLength;

  // Manually configure the GPIO pins based on your ESP32-S3 board and wiring
  // !! IMPORTANT: Replace these with your actual GPIO pin assignments !!
  // Standard A, B, C, D lines for 1/16 scan panels (32 high panels often use 4 address lines)
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

  // Create the display object using the config
  dma_display = new MatrixPanel_I2S_DMA(config);

  // Initialize display
  dma_display->begin();
  dma_display->setBrightness8(90); // Adjust brightness (0-255)
  dma_display->clearScreen();

  // Example display operations
  dma_display->setCursor(0, 0);
  dma_display->setTextSize(1);
  dma_display->setTextColor(dma_display->color565(255, 0, 0)); // Red text
  dma_display->print("Hello, Matrix!");
}

void loop() {
  // Your animation or display update code goes here
  // For example, to scroll text:
   dma_display->scrollText("Scrolling text example...", 1);
}