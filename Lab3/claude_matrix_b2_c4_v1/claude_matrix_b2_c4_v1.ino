/**********************************************************************
 * Filename    : claude_matrix_b2_c4_c2.ino
 * Description : Enhanced Pin-by-pin verification diagnostic for ESP32-S3 HUB75 Matrix connection
 * Author      : Rudy Martin / Next Shift Consulting - AUTO DIAGNOSTIC
 * Project     : Artemis DSAI 2025
 * Modified    : Jun 6, 2025   
 * PURPOSE     : AUTOMATIC continuous diagnostics with detailed serial output
 **********************************************************************/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

// ===========================================
// GPIO PIN CONFIGURATION FOR ESP32-S3
// ===========================================
// CUSTOM PIN MAPPING - ESP32-S3 OPTIMIZED
// These pins avoid UART, strapping, and sensitive pins

// Data Pins (RGB for upper and lower half)
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

// Compatibility aliases for the rest of the code
#define R1_PIN P_R1
#define G1_PIN P_G1
#define B1_PIN P_B1
#define R2_PIN P_R2
#define G2_PIN P_G2
#define B2_PIN P_B2
#define A_PIN P_A
#define B_PIN P_B
#define C_PIN P_C
#define D_PIN P_D
#define E_PIN P_E
#define LAT_PIN P_LAT
#define OE_PIN P_OE
#define CLK_PIN P_CLK

// Matrix configuration
#define PANEL_RES_X 64
#define PANEL_RES_Y 64
#define PANEL_CHAIN 1

// ===========================================
// TIMING CONFIGURATION
// ===========================================
#define TIME_ON_SCREEN 60000    // Time each pattern displays (milliseconds)
                                // 60000ms = 1 minute
                                // 30000ms = 30 seconds  
                                // 120000ms = 2 minutes
#define STATE_TRANSITION_TIME 3000  // Time for initial display tests (3 seconds)

// ===========================================
// FINITE STATE MACHINE STATES
// ===========================================
enum DisplayState {
  STATE_INIT,
  STATE_TESTING_PINS,
  STATE_DISPLAY_READY,
  STATE_RUNNING_PATTERN,
  STATE_ERROR
};

// ===========================================
// GLOBAL VARIABLES
// ===========================================
MatrixPanel_I2S_DMA *dma_display = nullptr;
DisplayState currentState = STATE_INIT;
unsigned long stateTimer = 0;
int testPattern = 0;

// Pin verification array
struct PinInfo {
  int pin;
  const char* name;
  bool isValid;
};

PinInfo pins[] = {
  {R1_PIN, "R1", false}, {G1_PIN, "G1", false}, {B1_PIN, "B1", false},
  {R2_PIN, "R2", false}, {G2_PIN, "G2", false}, {B2_PIN, "B2", false},
  {A_PIN, "A", false}, {B_PIN, "B", false}, {C_PIN, "C", false},
  {D_PIN, "D", false}, {E_PIN, "E", false},
  {LAT_PIN, "LAT", false}, {OE_PIN, "OE", false}, {CLK_PIN, "CLK", false}
};

// ===========================================
// SETUP FUNCTION
// ===========================================
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-S3 64x64 Matrix Display Initializing...");
  
  // Print current pin configuration
  printPinConfiguration();
  
  // Initialize state machine
  currentState = STATE_INIT;
  stateTimer = millis();
  
  // Run state machine
  runStateMachine();
}

// ===========================================
// MAIN LOOP
// ===========================================
void loop() {
  runStateMachine();
  delay(50); // Small delay to prevent overwhelming the system
}

// ===========================================
// STATE MACHINE IMPLEMENTATION
// ===========================================
void runStateMachine() {
  switch (currentState) {
    case STATE_INIT:
      handleInitState();
      break;
      
    case STATE_TESTING_PINS:
      handleTestingPinsState();
      break;
      
    case STATE_DISPLAY_READY:
      handleDisplayReadyState();
      break;
      
    case STATE_RUNNING_PATTERN:
      handleRunningPatternState();
      break;
      
    case STATE_ERROR:
      handleErrorState();
      break;
  }
}

// ===========================================
// STATE HANDLERS
// ===========================================
void handleInitState() {
  Serial.println("STATE: Initializing...");
  
  // Test if pins are available
  if (testPinAvailability()) {
    Serial.println("Pin test passed - transitioning to display setup");
    setupMatrixDisplay();
    currentState = STATE_TESTING_PINS;
  } else {
    Serial.println("Pin test failed - entering error state");
    currentState = STATE_ERROR;
  }
  stateTimer = millis();
}

void handleTestingPinsState() {
  Serial.println("STATE: Testing matrix display...");
  
  if (dma_display != nullptr) {
    // Test basic display functionality
    testBasicDisplay();
    
    if (millis() - stateTimer > STATE_TRANSITION_TIME) { // Use configurable time for testing
      Serial.println("Display test completed - ready for operation");
      currentState = STATE_DISPLAY_READY;
      stateTimer = millis();
    }
  } else {
    Serial.println("Display initialization failed");
    currentState = STATE_ERROR;
  }
}

void handleDisplayReadyState() {
  Serial.printf("STATE: Display ready - starting patterns (changing every %d seconds)\n", 
                TIME_ON_SCREEN / 1000);
  currentState = STATE_RUNNING_PATTERN;
  testPattern = 0;
  stateTimer = millis();
}

void handleRunningPatternState() {
  // Run different test patterns with configurable timing
  if (millis() - stateTimer > TIME_ON_SCREEN) { // Use configurable TIME_ON_SCREEN
    runTestPattern(testPattern);
    testPattern = (testPattern + 1) % 5; // Cycle through 5 patterns
    stateTimer = millis();
    
    // Print timing info
    Serial.printf("Pattern %d displayed for %d seconds\n", 
                  testPattern, TIME_ON_SCREEN / 1000);
  }
}

void handleErrorState() {
  Serial.println("STATE: ERROR - Check your wiring and pin configuration");
  
  // Blink built-in LED to indicate error
  static unsigned long errorBlink = 0;
  if (millis() - errorBlink > 500) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    errorBlink = millis();
  }
  
  // Try to recover every 10 seconds
  if (millis() - stateTimer > 10000) {
    Serial.println("Attempting recovery...");
    currentState = STATE_INIT;
    stateTimer = millis();
  }
}

// ===========================================
// PIN TESTING AND CONFIGURATION
// ===========================================
bool testPinAvailability() {
  Serial.println("\n=== PIN AVAILABILITY TEST ===");
  bool allPinsValid = true;
  
  for (int i = 0; i < sizeof(pins)/sizeof(pins[0]); i++) {
    pins[i].isValid = isValidGPIO(pins[i].pin);
    Serial.printf("Pin %d (%s): %s\n", 
                  pins[i].pin, 
                  pins[i].name, 
                  pins[i].isValid ? "VALID" : "INVALID");
    
    if (!pins[i].isValid) {
      allPinsValid = false;
    }
  }
  
  Serial.printf("Overall pin test: %s\n", allPinsValid ? "PASSED" : "FAILED");
  return allPinsValid;
}

bool isValidGPIO(int pin) {
  // ESP32-S3 specific GPIO validation
  // Valid GPIO pins for ESP32-S3: 0-21, 33-48
  // Your custom pins: 37, 38, 39, 40, 41, 42, 45, 47, 48 (high GPIO range)
  //                   15, 16, 17, 18, 19 (low GPIO range)
  
  if (pin >= 0 && pin <= 21) return true;
  if (pin >= 33 && pin <= 48) return true;
  
  // Reserved/problematic pins to avoid
  if (pin >= 22 && pin <= 25) return false; // SPI flash/PSRAM
  if (pin >= 26 && pin <= 32) return false; // Reserved/not available
  
  // Your pin configuration validation
  int validPins[] = {15, 16, 17, 18, 19, 37, 38, 39, 40, 41, 42, 45, 47, 48};
  for (int i = 0; i < sizeof(validPins)/sizeof(validPins[0]); i++) {
    if (pin == validPins[i]) return true;
  }
  
  return false;
}

void printPinConfiguration() {
  Serial.println("\n=== CUSTOM ESP32-S3 PIN CONFIGURATION ===");
  Serial.println("Data Pins:");
  Serial.printf("R1: GPIO%d  | G1: GPIO%d  | B1: GPIO%d\n", P_R1, P_G1, P_B1);
  Serial.printf("R2: GPIO%d  | G2: GPIO%d  | B2: GPIO%d\n", P_R2, P_G2, P_B2);
  Serial.println("Address Pins:");
  Serial.printf("A:  GPIO%d  | B:  GPIO%d  | C:  GPIO%d\n", P_A, P_B, P_C);
  Serial.printf("D:  GPIO%d  | E:  GPIO%d\n", P_D, P_E);
  Serial.println("Control Pins:");
  Serial.printf("LAT: GPIO%d | OE: GPIO%d | CLK: GPIO%d\n", P_LAT, P_OE, P_CLK);
  Serial.println("\nPin Selection Notes:");
  Serial.println("- Avoids UART pins (43, 44) used for USB Serial");
  Serial.println("- Avoids strapping pin 36 for CLK");
  Serial.println("- Uses high GPIO range (37-48) for data pins");
  Serial.println("- Uses safe low GPIO range (15-19) for control");
  Serial.println("==========================================\n");
}

// ===========================================
// MATRIX DISPLAY SETUP
// ===========================================
void setupMatrixDisplay() {
  Serial.println("Setting up matrix display...");
  
  HUB75_I2S_CFG::i2s_pins _pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
    A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
    LAT_PIN, OE_PIN, CLK_PIN
  };
  
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // width
    PANEL_RES_Y,   // height
    PANEL_CHAIN,   // chain length
    _pins          // pin mapping
  );
  
  // Additional configuration
  mxconfig.gpio.e = E_PIN;        // Required for 64 row displays
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  
  if (dma_display->begin()) {
    Serial.println("Matrix display initialized successfully!");
    dma_display->setBrightness8(90);
    dma_display->clearScreen();
  } else {
    Serial.println("Matrix display initialization FAILED!");
    delete dma_display;
    dma_display = nullptr;
  }
}

// ===========================================
// TEST PATTERNS
// ===========================================
void testBasicDisplay() {
  if (dma_display == nullptr) return;
  
  // Enhanced basic test - separate upper and lower halves
  dma_display->fillRect(0, 0, 64, 21, dma_display->color565(255, 0, 0));   // Red
  dma_display->fillRect(0, 21, 64, 21, dma_display->color565(0, 255, 0));  // Green  
  dma_display->fillRect(0, 42, 64, 22, dma_display->color565(0, 0, 255));  // Blue
  
  // Add diagnostic info
  Serial.println("Basic test: Red(0-20), Green(21-41), Blue(42-63)");
  Serial.println("Check if colors appear in correct regions");
}

void runTestPattern(int pattern) {
  if (dma_display == nullptr) return;
  
  dma_display->clearScreen();
  
  switch (pattern) {
    case 0: // RGB test with upper/lower half verification
      testUpperLowerHalves();
      Serial.println("Pattern: RGB Upper/Lower Half Test");
      break;
      
    case 1: // Individual color planes
      testColorPlanes();
      Serial.println("Pattern: Individual Color Planes");
      break;
      
    case 2: // Row addressing test
      testRowAddressing();
      Serial.println("Pattern: Row Addressing Test");
      break;
      
    case 3: // Checkerboard
      drawCheckerboard();
      Serial.println("Pattern: Checkerboard");
      break;
      
    case 4: // Text display
      drawText();
      Serial.println("Pattern: Text");
      break;
  }
}

// ===========================================
// ENHANCED DIAGNOSTIC TEST PATTERNS
// ===========================================
void testUpperLowerHalves() {
  // Test upper half (rows 0-31) - should use R1,G1,B1
  dma_display->fillRect(0, 0, 64, 32, dma_display->color565(255, 0, 0));   // Red upper
  
  // Test lower half (rows 32-63) - should use R2,G2,B2  
  dma_display->fillRect(0, 32, 64, 32, dma_display->color565(0, 255, 0));  // Green lower
  
  // Add a white line at the split to clearly show the division
  dma_display->drawLine(0, 31, 63, 31, dma_display->color565(255, 255, 255));
  dma_display->drawLine(0, 32, 63, 32, dma_display->color565(255, 255, 255));
}

void testColorPlanes() {
  static int colorTest = 0;
  
  switch (colorTest % 6) {
    case 0: // Red only - upper half
      dma_display->fillRect(0, 0, 64, 32, dma_display->color565(255, 0, 0));
      Serial.println("  Testing: RED upper half (R1 pin)");
      break;
    case 1: // Red only - lower half  
      dma_display->fillRect(0, 32, 64, 32, dma_display->color565(255, 0, 0));
      Serial.println("  Testing: RED lower half (R2 pin)");
      break;
    case 2: // Green only - upper half
      dma_display->fillRect(0, 0, 64, 32, dma_display->color565(0, 255, 0));
      Serial.println("  Testing: GREEN upper half (G1 pin)");
      break;
    case 3: // Green only - lower half
      dma_display->fillRect(0, 32, 64, 32, dma_display->color565(0, 255, 0));
      Serial.println("  Testing: GREEN lower half (G2 pin)");
      break;
    case 4: // Blue only - upper half
      dma_display->fillRect(0, 0, 64, 32, dma_display->color565(0, 0, 255));
      Serial.println("  Testing: BLUE upper half (B1 pin)");
      break;
    case 5: // Blue only - lower half
      dma_display->fillRect(0, 32, 64, 32, dma_display->color565(0, 0, 255));
      Serial.println("  Testing: BLUE lower half (B2 pin)");
      break;
  }
  colorTest++;
}

void testRowAddressing() {
  // Draw individual rows to test addressing
  for (int row = 0; row < 64; row += 8) {
    uint16_t color = dma_display->color565(
      (row * 4) % 256,          // Red gradient
      ((64 - row) * 4) % 256,   // Green gradient  
      128                        // Constant blue
    );
    dma_display->drawLine(0, row, 63, row, color);
    dma_display->drawLine(0, row + 1, 63, row + 1, color);
  }
  
  // Add row numbers every 16 rows
  dma_display->setTextColor(dma_display->color565(255, 255, 255));
  dma_display->setTextSize(1);
  for (int row = 0; row < 64; row += 16) {
    dma_display->setCursor(2, row + 2);
    dma_display->printf("%d", row);
  }
}

void drawRainbow() {
  for (int x = 0; x < 64; x++) {
    int hue = map(x, 0, 63, 0, 255);
    CRGB color = CHSV(hue, 255, 255);
    uint16_t color565 = dma_display->color565(color.r, color.g, color.b);
    dma_display->drawLine(x, 0, x, 63, color565);
  }
}

void drawCheckerboard() {
  for (int x = 0; x < 64; x += 8) {
    for (int y = 0; y < 64; y += 8) {
      uint16_t color = ((x/8 + y/8) % 2) ? 
        dma_display->color565(255, 255, 255) : 
        dma_display->color565(0, 0, 0);
      dma_display->fillRect(x, y, 8, 8, color);
    }
  }
}

void drawMovingPixel() {
  static int pixelX = 0, pixelY = 0;
  dma_display->drawPixel(pixelX, pixelY, dma_display->color565(255, 255, 0));
  pixelX = (pixelX + 1) % 64;
  if (pixelX == 0) pixelY = (pixelY + 1) % 64;
}

void drawText() {
  dma_display->setTextColor(dma_display->color565(0, 255, 255));
  dma_display->setTextSize(1);
  dma_display->setCursor(2, 28);
  dma_display->print("ESP32-S3");
  dma_display->setCursor(8, 38);
  dma_display->print("64x64");
}

// ===========================================
// DIAGNOSTIC FUNCTIONS
// ===========================================
void printDiagnostics() {
  Serial.println("\n=== DIAGNOSTIC INFORMATION ===");
  Serial.printf("ESP32 Chip: %s\n", ESP.getChipModel());
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Current State: %d\n", currentState);
  Serial.printf("Display Initialized: %s\n", (dma_display != nullptr) ? "YES" : "NO");
  
  if (dma_display != nullptr) {
    Serial.println("Matrix Display: ACTIVE");
    Serial.println("\n=== TROUBLESHOOTING GUIDE ===");
    Serial.println("If you see red bands in upper portion only:");
    Serial.println("1. Check R2, G2, B2 wiring (lower half data)");
    Serial.println("2. Verify address pins A, B, C, D, E connections");
    Serial.println("3. Check LAT and OE pin connections");
    Serial.println("4. Ensure proper power supply (5V, adequate current)");
    Serial.println("5. Verify CLK signal integrity");
  } else {
    Serial.println("Matrix Display: FAILED - Check wiring and pins!");
  }
  Serial.println("===============================\n");
}
