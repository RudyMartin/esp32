/**********************************************************************
 * matrix_setup_blink_wait.ino
 * Pin-by-pin verification for ESP32-S3 HUB75 Matrix connection
 * Tests each pin individually to verify wiring
 **********************************************************************/

// Pin configurations for Freenove ESP32-S3 board
// Configuration 1: Standard ESP32-S3 pins
struct PinConfig {
  int pin;
  String name;
  String function;
  String ribonColor;
};

// Configuration 1: ESP32-S3 Standard (adjust these to match your setup)
PinConfig config1[] = {
  {25, "R1", "Red Upper", "Blue"},      // Pin 1
  {26, "G1", "Green Upper", "Green"},   // Pin 2
  {27, "B1", "Blue Upper", "Yellow"},   // Pin 3
  {14, "R2", "Red Lower", "Orange"},    // Pin 4
  {12, "G2", "Green Lower", "Red"},     // Pin 5
  {13, "B2", "Blue Lower", "Brown"},    // Pin 6
  {23, "A", "Address A", "Black"},      // Pin 7
  {19, "B", "Address B", "White"},      // Pin 8
  {5, "C", "Address C", "Silver"},      // Pin 9
  {17, "D", "Address D", "Purple"},     // Pin 10
  {18, "E", "Address E", "Blue"},       // Pin 11
  {-1, "GND", "Ground", "Green"},       // Pin 12 (Ground - not testable)
  {4, "LAT", "Latch", "Yellow"},        // Pin 13
  {15, "OE", "Output Enable", "Orange"}, // Pin 14
  {16, "CLK", "Clock", "Red"},          // Pin 15
  {-1, "5V", "Power 5V", "Brown"}       // Pin 16 (Power - not testable)
};

// Configuration 2: Alternative ESP32-S3 pins
PinConfig config2[] = {
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
  
  Serial.println("Starting pin verification test...");
  Serial.println("Watch your matrix and multimeter for activity on each pin!\n");
}

void loop() {
  PinConfig* activeConfig = (ACTIVE_CONFIG == 1) ? config1 : config2;
  int configSize = 16;
  
  // Test each pin 25 times as requested
  for(int cycle = 1; cycle <= 25; cycle++) {
    Serial.println("=== CYCLE " + String(cycle) + " of 25 ===");
    
    for(int i = 0; i < configSize; i++) {
      if(activeConfig[i].pin != -1) {  // Skip GND and 5V pins
        
        Serial.println("Testing Pin " + String(i+1) + ": " + 
                      activeConfig[i].name + " (" + activeConfig[i].function + 
                      ") - GPIO" + String(activeConfig[i].pin) + 
                      " - Ribbon: " + activeConfig[i].ribonColor);
        
        // Give you time to read the pin info
        Serial.println("  -> Starting test in 3 seconds...");
        delay(3000);
        
        // Blink the pin HIGH for 2 seconds, then LOW for 2 seconds
        digitalWrite(activeConfig[i].pin, HIGH);
        Serial.println("  -> GPIO" + String(activeConfig[i].pin) + " = HIGH (3.3V) - MEASURE NOW!");
        delay(2000);
        
        digitalWrite(activeConfig[i].pin, LOW);
        Serial.println("  -> GPIO" + String(activeConfig[i].pin) + " = LOW (0V) - MEASURE NOW!");
        delay(2000);
        
        Serial.println("  -> Check: Did you see voltage change on " + 
                      activeConfig[i].ribonColor + " wire?");
        Serial.println("  -> Press Enter in Serial Monitor when ready for next pin...");
        
        // Wait for user input before continuing
        while(!Serial.available()) {
          delay(100);
        }
        while(Serial.available()) {
          Serial.read(); // Clear the buffer
        }
        Serial.println("");
        
      } else {
        Serial.println("Skipping Pin " + String(i+1) + ": " + 
                      activeConfig[i].name + " (" + activeConfig[i].function + 
                      ") - Ribbon: " + activeConfig[i].ribonColor + 
                      " (Power/Ground pin)");
        delay(200);  // Short pause for power pins
        Serial.println("");
      }
    }
    
    Serial.println("Cycle " + String(cycle) + " complete. Pausing 2 seconds...\n");
    delay(2000);
  }
  
  Serial.println("🎉 ALL 25 CYCLES COMPLETE! 🎉");
  Serial.println("\nPin verification test finished.");
  Serial.println("Review the output above to confirm each pin matches your wiring.");
  Serial.println("\nTo test the other configuration, change ACTIVE_CONFIG to " + 
                String(ACTIVE_CONFIG == 1 ? 2 : 1) + " and restart.");
  
  // Infinite pause
  while(true) {
    delay(10000);
  }
}

// Quick reference for your ribbon cable colors:
/*
Your 16-pin ribbon colors in order:
1.  Blue
2.  Green  
3.  Yellow
4.  Orange
5.  Red
6.  Brown
7.  Black
8.  White
9.  Silver
10. Purple
11. Blue (repeat)
12. Green (repeat)
13. Yellow (repeat)
14. Orange (repeat)
15. Red (repeat)
16. Brown (repeat)

Standard HUB75 pinout:
1.  R1    (Red Upper)
2.  G1    (Green Upper)
3.  B1    (Blue Upper)
4.  R2    (Red Lower)
5.  G2    (Green Lower)
6.  B2    (Blue Lower)
7.  A     (Address A)
8.  B     (Address B)
9.  C     (Address C)
10. D     (Address D)
11. E     (Address E, for 64x64)
12. GND   (Ground)
13. LAT   (Latch)
14. OE    (Output Enable)
15. CLK   (Clock)
16. 5V    (Power)
*/