*********************************************************************
 * matrix_setup_blink_wait.ino
 * Enhanced Pin-by-pin verification for ESP32-S3 HUB75 Matrix connection
 * Tests each pin individually with extended timing and enhanced blinking
 * IBROKE THIS
 **********************************************************************/

// Pin configurations for Freenove ESP32-S3 board
// Configuration 1: Standard ESP32-S3 pins
struct PinConfig {
  int pin;
  String name;
  String function;
  String ribonColor;
};

 config4
Thoughts
Expand to show model's summarized thoughts
Ah, okay! That line tells me you are using the PxMatrix library (from 2m and collaborative efforts). This is a very important detail, as PxMatrix handles pin assignments differently than a generic PinConfig struct.

The PxMATRIX constructor:
PxMATRIX display(matrix_width,matrix_height,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

...only takes the control and address pins as arguments. The data pins (R1, G1, B1, R2, G2, B2) and the Clock pin (CLK) are typically defined as preprocessor macros (#define) before you include the PxMatrix header. The library then uses these defined pins internally to set up the I2S or RMT peripheral for high-speed data transfer.

This is why your previous PinConfig structure isn't directly compatible with how PxMatrix expects its pins!

Let's translate my previous recommended safe pins into the format PxMatrix expects, and provide a full example.

Understanding PxMatrix Pin Assignment:

Data Pins (R1, G1, B1, R2, G2, B2) and CLK: Defined using #define.
Control Pins (LAT, OE, A, B, C, D, E): Passed directly into the PxMATRIX constructor.
Recommended Pin Configuration for Freenove ESP32-S3 with PxMatrix:

I'll use Option 1a from my previous reply, as it specifically avoids the known problematic pins on the S3 while keeping your working RGB pins.

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


// // Configuration 3: Huh - not tested ESP32-S3 pins
// PinConfig config3[] = {
//   {42, "R1", "Red Upper", "Blue"},      // Pin 1
//   {41, "G1", "Green Upper", "Green"},   // Pin 2
//   {40, "B1", "Blue Upper", "Yellow"},   // Pin 3
//   {38, "R2", "Red Lower", "Orange"},    // Pin 4
//   {39, "G2", "Green Lower", "Red"},     // Pin 5
//   {37, "B2", "Blue Lower", "Brown"},    // Pin 6
//   {45, "A", "Address A", "Black"},      // Pin 7
//   {48, "B", "Address B", "White"},      // Pin 8
//   {47, "C", "Address C", "Silver"},     // Pin 9
//   {21, "D", "Address D", "Purple"},     // Pin 10
//   {14, "E", "Address E", "Blue"},       // Pin 11
//   {-1, "GND", "Ground", "Green"},       // Pin 12 (Ground)
//   {2, "LAT", "Latch", "Yellow"},        // Pin 13
//   {1, "OE", "Output Enable", "Orange"}, // Pin 14
//   {36, "CLK", "Clock", "Red"},          // Pin 15
//   {-1, "5V", "Power 5V", "Brown"}       // Pin 16 (Power)
// };

// // Configuration 2: Huh Not tested - ESP32-S3 Standard (adjust these to match your setup)
// PinConfig config2[] = {
//   {25, "R1", "Red Upper", "Blue"},      // Pin 1
//   {26, "G1", "Green Upper", "Green"},   // Pin 2
//   {27, "B1", "Blue Upper", "Yellow"},   // Pin 3
//   {14, "R2", "Red Lower", "Orange"},    // Pin 4
//   {12, "G2", "Green Lower", "Red"},     // Pin 5
//   {13, "B2", "Blue Lower", "Brown"},    // Pin 6
//   {23, "A", "Address A", "Black"},      // Pin 7
//   {19, "B", "Address B", "White"},      // Pin 8
//   {5, "C", "Address C", "Silver"},      // Pin 9
//   {17, "D", "Address D", "Purple"},     // Pin 10
//   {18, "E", "Address E", "Blue"},       // Pin 11
//   {-1, "GND", "Ground", "Green"},       // Pin 12 (Ground - not testable)
//   {4, "LAT", "Latch", "Yellow"},        // Pin 13
//   {15, "OE", "Output Enable", "Orange"}, // Pin 14
//   {16, "CLK", "Clock", "Red"},          // Pin 15
//   {-1, "5V", "Power 5V", "Brown"}       // Pin 16 (Power - not testable)
// };



// Configuration 1: Old ESP32-S3 pins
PinConfig config1[] = {
  {25, "R1", "Red Upper", "Blue"},      // Pin 1
  {26, "G1", "Green Upper", "Green"},   // Pin 2
  {27, "B1", "Blue Upper", "Yellow"},   // Pin 3
  {39, "R2", "Red Lower", "Orange"},    // Pin 4
  {38, "G2", "Green Lower", "Red"},     // Pin 5
  {37, "B2", "Blue Lower", "Brown"},    // Pin 6
  {45, "A", "Address A", "Black"},      // Pin 7
  {48, "B", "Address B", "White"},      // Pin 8
  {47, "C", "Address C", "Silver"},     // Pin 9
  {15, "D", "Address D", "Purple"},     // Pin 10
  {16, "E", "Address E", "Blue"},       // Pin 11
  {-1, "GND", "Ground", "Green"},       // Pin 12 (Ground)
  {17, "LAT", "Latch", "Yellow"},        // Pin 13
  {18, "OE", "Output Enable", "Orange"}, // Pin 14
  {19, "CLK", "Clock", "Red"},          // Pin 15
  {-1, "5V", "Power 5V", "Brown"}       // Pin 16 (Power)
};

// Configuration 2: Recommended Safe Pins (ESP32-S3, Freenove focused)
PinConfig config2[] = {
  {42, "R1", "Red Upper", "Blue"},     // Pin 1 (Keep working RGB)
  {41, "G1", "Green Upper", "Green"},  // Pin 2 (Keep working RGB)
  {40, "B1", "Blue Upper", "Yellow"},  // Pin 3 (Keep working RGB)
  {39, "R2", "Red Lower", "Orange"},   // Pin 4 (Keep working RGB)
  {38, "G2", "Green Lower", "Red"},    // Pin 5 (Keep working RGB)
  {37, "B2", "Blue Lower", "Brown"},   // Pin 6 (Keep working RGB)
  {45, "A", "Address A", "Black"},     // Pin 7 (Keep, generally fine)
  {48, "B", "Address B", "White"},     // Pin 8 (Keep, generally fine)
  {47, "C", "Address C", "Silver"},    // Pin 9 (Keep, generally fine)
  {15, "D", "Address D", "Purple"},    // Pin 10 (Changed from 21: Safer GPIO)
  {16, "E", "Address E", "Blue"},      // Pin 11 (Changed from 14: Safer GPIO)
  {-1, "GND", "Ground", "Green"},      // Pin 12 (Ground)
  {17, "LAT", "Latch", "Yellow"},      // Pin 13 (Changed from 2: Avoids UART TX)
  {18, "OE", "Output Enable", "Orange"},// Pin 14 (Changed from 1: Avoids UART RX)
  {19, "CLK", "Clock", "Red"},         // Pin 15 (Changed from 36: Safer, contiguous)
  {-1, "5V", "Power 5V", "Brown"}      // Pin 16 (Power)
};

// Configuration 3: Jittery Pins (ESP32-S3, Freenove focused)
PinConfig config3[] = {
  {42, "R1", "Red Upper", "Blue"},     // Pin 1 (Keep working RGB)
  {41, "G1", "Green Upper", "Green"},  // Pin 2 (Keep working RGB)
  {40, "B1", "Blue Upper", "Yellow"},  // Pin 3 (Keep working RGB)
  {39, "R2", "Red Lower", "Orange"},   // Pin 4 (Keep working RGB)
  {38, "G2", "Green Lower", "Red"},    // Pin 5 (Keep working RGB)
  {37, "B2", "Blue Lower", "Brown"},   // Pin 6 (Keep working RGB)
  {45, "A", "Address A", "Black"},     // Pin 7 (Keep, generally fine)
  {48, "B", "Address B", "White"},     // Pin 8 (Keep, generally fine)
  {47, "C", "Address C", "Silver"},    // Pin 9 (Keep, generally fine)
  {21, "D", "Address D", "Purple"},    // Pin 10 (Changed from 21: Safer GPIO)
  {14, "E", "Address E", "Blue"},      // Pin 11 (Changed from 14: Safer GPIO)
  {-1, "GND", "Ground", "Green"},      // Pin 12 (Ground)
  {2, "LAT", "Latch", "Yellow"},      // Pin 13 (Changed from 2: Avoids UART TX)
  {1, "OE", "Output Enable", "Orange"},// Pin 14 (Changed from 1: Avoids UART RX)
  {36, "CLK", "Clock", "Red"},         // Pin 15 (Changed from 36: Safer, contiguous)
  {-1, "5V", "Power 5V", "Brown"}      // Pin 16 (Power)
// Choose which configuration to test (change this to 1 or 2)
const int ACTIVE_CONFIG = 3;  // Set to 1 for config1, 2 for config2

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("\n===========================================");
  Serial.println("ESP32-S3 HUB75 Matrix Pin Verification Test");
  Serial.println("ENHANCED VERSION - Extended Timing & Blinking");
  Serial.println("===========================================");
  
  PinConfig* activeConfig = (ACTIVE_CONFIG == 1) ? config1 : config2;
  int configSize = (ACTIVE_CONFIG == 1) ? 16 : 16;
  
  Serial.println("Configuration " + String(ACTIVE_CONFIG) + " Pin Mapping:");
  Serial.println("Ribbon Pin | ESP32 Pin | Function | Ribbon Color | Signal Name");
  Serial.println("-----------|-----------|----------|--------------|------------");
  
  for(int i = 0; i < configSize; i++) {
    String pinStr = (activeConfig[i].pin == -1) ? "N/A" : String(activeConfig[i].pin);
    Serial.printf("    %2d     |    %3s    |   %-6s |     %-6s   | %s\n", 
                  i+1, pinStr.c_str(), activeConfig[i].name.c_str(), 
                  activeConfig[i].ribonColor.c_str(), activeConfig[i].function.c_str());
  }
  
  Serial.println("\nNOTE: GND and 5V pins cannot be tested with digitalWrite");
  Serial.println("Make sure these are connected to ground and 5V power!\n");
  
  // Initialize testable pins as outputs
  for(int i = 0; i < configSize; i++) {
    if(activeConfig[i].pin != -1) {
      pinMode(activeConfig[i].pin, OUTPUT);
      digitalWrite(activeConfig[i].pin, LOW);
    }
  }
  
  Serial.println("Starting enhanced pin verification test...");
  Serial.println("Each pin will blink multiple times with extended delays!");
  Serial.println("Watch your matrix and multimeter for activity on each pin!\n");
  
  Serial.println("*** IMPORTANT INSTRUCTIONS ***");
  Serial.println("1. Have your multimeter ready to measure voltage");
  Serial.println("2. Connect multimeter probe to the ribbon wire color shown");
  Serial.println("3. Watch for voltage changes: 0V -> 3.3V -> 0V");
  Serial.println("4. Each pin gets 30 seconds of reading time before blinking");
  Serial.println("5. Enhanced blinking pattern for better visibility");
  Serial.println("==============================\n");
}

void enhancedBlink(int pin, String pinName, int blinkCount = 5) {
  Serial.println("  *** ENHANCED BLINKING SEQUENCE STARTING ***");
  
  for(int blink = 1; blink <= blinkCount; blink++) {
    Serial.println("    Blink " + String(blink) + "/" + String(blinkCount) + ":");
    
    // HIGH phase - longer duration for easier measurement
    digitalWrite(pin, HIGH);
    Serial.println("      -> GPIO" + String(pin) + " = HIGH (3.3V) - MEASURE NOW!");
    delay(3000);  // 3 seconds HIGH
    
    // LOW phase
    digitalWrite(pin, LOW);
    Serial.println("      -> GPIO" + String(pin) + " = LOW (0V) - MEASURE NOW!");
    delay(2000);  // 2 seconds LOW
    
    if(blink < blinkCount) {
      Serial.println("      -> Next blink in 1 second...");
      delay(1000);
    }
  }
  
  Serial.println("  *** BLINKING SEQUENCE COMPLETE ***");
}

void matrixBlink(int pin, String pinName, int blinkCount = 10) {
  Serial.println("  *** MATRIX LED BLINKING SEQUENCE STARTING ***");
  Serial.println("  -> Watch the matrix display for LED activity!");
  
  for(int blink = 1; blink <= blinkCount; blink++) {
    Serial.println("    Matrix Blink " + String(blink) + "/" + String(blinkCount) + " on GPIO" + String(pin));
    
    // Fast blink pattern - like your switch case BLINK
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
  }
  
  Serial.println("  *** MATRIX BLINKING SEQUENCE COMPLETE ***");
  Serial.println("  -> Did you see LED activity on the matrix display?");
}

void combinedPinTest(int pin, String pinName, String ribonColor, String function) {
  Serial.println("\n*** COMBINED PIN TEST: GPIO Voltage + Matrix LED ***");
  
  // First: Fast matrix LED blinking for visual confirmation
  Serial.println("PHASE 1: Matrix LED Blinking Test");
  Serial.println("-> Look at your matrix display for LED activity...");
  delay(2000);
  matrixBlink(pin, pinName, 10);
  
  delay(1000);
  
  // Second: Slow voltage testing for multimeter measurement
  Serial.println("\nPHASE 2: Voltage Measurement Test");
  Serial.println("-> Connect your multimeter to the " + ribonColor + " wire...");
  delay(2000);
  enhancedBlink(pin, pinName, 3);
  
  Serial.println("\n*** COMBINED TEST COMPLETE ***");
}

void loop() {
  PinConfig* activeConfig = (ACTIVE_CONFIG == 1) ? config1 : config2;
  int configSize = 16;
  
  // Test each pin 25 times as requested
  for(int cycle = 1; cycle <= 25; cycle++) {
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.println("║         CYCLE " + String(cycle) + " of 25             ║");
    Serial.println("╚═══════════════════════════════════════╝");
    
    for(int i = 0; i < configSize; i++) {
      if(activeConfig[i].pin != -1) {  // Skip GND and 5V pins
        
        Serial.println("\n┌─────────────────────────────────────────────────────────┐");
        Serial.println("│ Testing Pin " + String(i+1) + ": " + activeConfig[i].name + " (" + activeConfig[i].function + ")");
        Serial.println("│ ESP32 GPIO: " + String(activeConfig[i].pin));
        Serial.println("│ Ribbon Wire Color: " + activeConfig[i].ribonColor);
        Serial.println("└─────────────────────────────────────────────────────────┘");
        
        // Extended reading time - 30 seconds as requested
        Serial.println("\n*** 30 SECOND READING PERIOD ***");
        Serial.println("Prepare your multimeter on the " + activeConfig[i].ribonColor + " wire.");
        Serial.println("You have 30 seconds to get ready before blinking starts...");
        
        // Countdown timer for user preparation
        for(int countdown = 30; countdown > 0; countdown--) {
          if(countdown <= 10 || countdown % 5 == 0) {
            Serial.println("Starting in " + String(countdown) + " seconds... (Connect probe to " + 
                          activeConfig[i].ribonColor + " wire)");
          }
          delay(1000);
        }
        
        Serial.println("\n🚨 COMBINED TESTING STARTS NOW! 🚨");
        
        // Combined test: Matrix LED blinking + Voltage measurement
        combinedPinTest(activeConfig[i].pin, activeConfig[i].name, 
                       activeConfig[i].ribonColor, activeConfig[i].function);
        
        // Verification prompt
        Serial.println("\n*** VERIFICATION QUESTIONS ***");
        Serial.println("1. Did you see voltage change from 0V to 3.3V and back?");
        Serial.println("2. Was the " + activeConfig[i].ribonColor + " wire responding correctly?");
        Serial.println("3. Did the timing match the serial output?");
        
        Serial.println("\n>>> Press Enter when ready to continue, or wait 15 seconds for auto-advance...");
        
        // Extended wait for user input OR 15 second timeout
        unsigned long waitStart = millis();
        bool userResponded = false;
        
        while((millis() - waitStart < 15000) && !userResponded) {
          if(Serial.available()) {
            userResponded = true;
            while(Serial.available()) {
              Serial.read(); // Clear the buffer
            }
            Serial.println("✓ User input received, continuing to next pin...");
          }
          delay(100);
        }
        
        if(!userResponded) {
          Serial.println("⏰ 15 second timeout reached, auto-advancing to next pin...");
        }
        
        // Ensure pin is LOW before moving to next
        digitalWrite(activeConfig[i].pin, LOW);
        Serial.println("Pin reset to LOW state.\n");
        
      } else {
        Serial.println("\n┌─────────────────────────────────────────────────────────┐");
        Serial.println("│ Skipping Pin " + String(i+1) + ": " + activeConfig[i].name + " (" + activeConfig[i].function + ")");
        Serial.println("│ Ribbon Wire Color: " + activeConfig[i].ribonColor);
        Serial.println("│ Reason: Power/Ground pin - cannot be tested with GPIO");
        Serial.println("└─────────────────────────────────────────────────────────┘");
        Serial.println("*** MANUAL CHECK REQUIRED ***");
        Serial.println("Verify " + activeConfig[i].ribonColor + " wire is connected to " + activeConfig[i].function);
        delay(3000);  // Give time to read the message
        Serial.println("");
      }
    }
    
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.println("║  Cycle " + String(cycle) + " Complete! Pausing 5 seconds...  ║");
    Serial.println("╚═══════════════════════════════════════╝\n");
    delay(5000);  // Extended pause between cycles
  }
  
  Serial.println("🎉🎉🎉 ALL 25 CYCLES COMPLETE! 🎉🎉🎉");
  Serial.println("\n╔══════════════════════════════════════════╗");
  Serial.println("║         TEST SUMMARY & NEXT STEPS        ║");
  Serial.println("╚══════════════════════════════════════════╝");
  Serial.println("✓ Pin verification test finished successfully");
  Serial.println("✓ Each pin was tested with enhanced blinking pattern");
  Serial.println("✓ Extended timing allowed for proper measurement");
  Serial.println("\n📋 REVIEW CHECKLIST:");
  Serial.println("□ All color-coded wires responded to voltage changes");
  Serial.println("□ Multimeter readings matched serial output timing");
  Serial.println("□ No shorts or crossed connections detected");
  Serial.println("□ Power and ground connections verified manually");
  
  Serial.println("\n🔄 To test the other configuration:");
  Serial.println("Change ACTIVE_CONFIG to " + String(ACTIVE_CONFIG == 1 ? 2 : 1) + " and restart.");
  
  Serial.println("\n🔧 If any pins failed verification:");
  Serial.println("- Check ribbon cable seating");
  Serial.println("- Verify wire continuity with multimeter");
  Serial.println("- Confirm ESP32 pin assignments in code");
  
  Serial.println("\n💤 Test complete. System entering infinite pause...");
  
  // Infinite pause with periodic status
  unsigned long statusTimer = millis();
  while(true) {
    if(millis() - statusTimer > 60000) {  // Every minute
      Serial.println("⏰ System still running. Pin test complete. Reset to run again.");
      statusTimer = millis();
    }
    delay(10000);
  }
}

// Quick reference for your ribbon cable colors:
/*
Your 16-pin ribbon colors in order:
1.  Blue      -> R1    (Red Upper)
2.  Green     -> G1    (Green Upper)  
3.  Yellow    -> B1    (Blue Upper)
4.  Orange    -> R2    (Red Lower)
5.  Red       -> G2    (Green Lower)
6.  Brown     -> B2    (Blue Lower)
7.  Black     -> A     (Address A)
8.  White     -> B     (Address B)
9.  Silver    -> C     (Address C)
10. Purple    -> D     (Address D)
11. Blue      -> E     (Address E, for 64x64)
12. Green     -> GND   (Ground) - MANUAL CHECK
13. Yellow    -> LAT   (Latch)
14. Orange    -> OE    (Output Enable)
15. Red       -> CLK   (Clock)
16. Brown     -> 5V    (Power) - MANUAL CHECK

ENHANCED FEATURES IN THIS VERSION:
✓ 30-second preparation time before each pin test
✓ Enhanced blinking: 5 blinks per pin with 3s HIGH, 2s LOW
✓ Extended user wait time: 15 seconds (vs 5 seconds)
✓ Better visual formatting with boxes and symbols
✓ Detailed verification prompts and checklists
✓ Countdown timers for better user experience
✓ Status messages every minute during infinite pause
*/