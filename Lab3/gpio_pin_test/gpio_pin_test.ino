/*
 * ESP32 GPIO Pin Tester
 * Tests each pin used for the LED matrix to verify functionality
 * Connect an LED + resistor (220-470 ohm) between each GPIO and GND to see output
 * Or use a multimeter to measure voltage changes
 */

// Define all the pins we're testing (same as matrix config)
const int pins[] = {25, 26, 27, 14, 12, 13, 23, 22, 5, 17, 33, 4, 15, 16};
const char* pinNames[] = {"R1", "G1", "B1", "R2", "G2", "B2", "A", "B", "C", "D", "E", "LAT", "OE", "CLK"};
const int numPins = sizeof(pins) / sizeof(pins[0]);

// Built-in LED pin (varies by board)
const int BUILTIN_LED = 2; // GPIO 2 for most ESP32-S3 boards

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== ESP32 GPIO Pin Tester ===");
  Serial.println("This will test each GPIO pin used for the LED matrix");
  Serial.println("Connect LEDs+resistors to see visual confirmation");
  Serial.println();

  // Initialize built-in LED
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  // Initialize all test pins as outputs
  for (int i = 0; i < numPins; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
    Serial.print("Initialized GPIO ");
    Serial.print(pins[i]);
    Serial.print(" (");
    Serial.print(pinNames[i]);
    Serial.println(")");
  }

  Serial.println();
  Serial.println("Starting pin tests...");
  Serial.println("Watch for built-in LED blinks and measure voltages on each pin");
}

void loop() {
  // Test each pin individually
  for (int i = 0; i < numPins; i++) {
    Serial.println("========================================");
    Serial.print("Testing GPIO ");
    Serial.print(pins[i]);
    Serial.print(" (");
    Serial.print(pinNames[i]);
    Serial.println(" pin)");
    
    // Flash built-in LED to show which pin we're testing
    flashBuiltinLED(3);
    
    Serial.println("Pin should go HIGH for 2 seconds...");
    digitalWrite(pins[i], HIGH);
    delay(2000);
    
    Serial.println("Pin should go LOW for 1 second...");
    digitalWrite(pins[i], LOW);
    delay(1000);
    
    // Test rapid blinking
    Serial.println("Rapid blinking test (10 times)...");
    for (int j = 0; j < 10; j++) {
      digitalWrite(pins[i], HIGH);
      delay(100);
      digitalWrite(pins[i], LOW);
      delay(100);
    }
    
    Serial.print("GPIO ");
    Serial.print(pins[i]);
    Serial.println(" test complete");
    Serial.println("Did you see voltage changes? (Y/N)");
    Serial.println();
    
    delay(1000); // Pause between pins
  }
  
  Serial.println("========================================");
  Serial.println("ALL PIN TESTS COMPLETE");
  Serial.println("Results summary:");
  Serial.println("- Any pins that didn't change voltage are faulty");
  Serial.println("- GPIO 32 (E pin) is most critical for your matrix issue");
  Serial.println("- Check your wiring for any non-working pins");
  Serial.println();
  Serial.println("Starting test cycle again in 5 seconds...");
  Serial.println("========================================");
  
  delay(5000);
}

void flashBuiltinLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(200);
    digitalWrite(BUILTIN_LED, LOW);
    delay(200);
  }
}

/*
 * TESTING INSTRUCTIONS:
 * 
 * 1. Upload this code to your ESP32
 * 2. Open Serial Monitor (115200 baud)
 * 3. For each pin test:
 *    - Use multimeter to measure voltage between GPIO pin and GND
 *    - Should see 0V when LOW, ~3.3V when HIGH
 *    - OR connect LED+resistor between GPIO and GND to see visual indication
 * 
 * 4. Pay special attention to GPIO 32 (E pin) - this is likely your problem
 * 
 * 5. If any pin doesn't change voltage:
 *    - Try a different GPIO pin
 *    - Check for loose connections
 *    - Pin might be damaged
 * 
 * WIRING FOR VISUAL TEST:
 * Connect between each GPIO and GND:
 * LED (long leg to GPIO) -> 330 ohm resistor -> GND
 */
