/**********************************************************************
 * matrix_setup_blink_wait.ino
 * Enhanced Pin-by-pin verification for ESP32-S3 HUB75 Matrix connection
 * Tests each pin individually with extended timing and enhanced blinking
 **********************************************************************/

// Pin configurations for Freenove ESP32-S3 board
// Configuration 1: Standard ESP32-S3 pins
struct PinConfig {
  int pin;
  String name;
  String function;
  String ribonColor;
};

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

// Configuration 1: (Recommended) ESP32-S3 pins
PinConfig config1[] = {
  {42, "R1", "Red Upper", "Blue"},      // Pin 1
  {41, "G1", "Green Upper", "Green"},   // Pin 2
  {40, "B1", "Blue Upper", "Yellow"},   // Pin 3
  {38, "R2", "Red Lower", "Orange"},    // Pin 4
  {39, "G2", "Green Lower", "Red"},     // Pin 5
  {37, "B2", "Blue Lower", "Brown"},    // Pin 6
  {45, "A", "Address A", "Black"},      // Pin 7
  {48, "B", "Address B", "White"},      // Pin 8
  {47, "C", "Address C", "Silver"},     // Pin 9
  {21, "D", "Address D", "Purple"},     // Pin 10
  {14, "E", "Address E", "Blue"},       // Pin 11
  {-1, "GND", "Ground", "Green"},       // Pin 12 (Ground)
  {2, "LAT", "Latch", "Yellow"},        // Pin 13
  {1, "OE", "Output Enable", "Orange"}, // Pin 14
  {36, "CLK", "Clock", "Red"},          // Pin 15
  {-1, "5V", "Power 5V", "Brown"}       // Pin 16 (Power)
};

// Choose which configuration to test (change this to 1 or 2)
const int ACTIVE_CONFIG = 1;  // Set to 1 for config1, 2 for config2

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