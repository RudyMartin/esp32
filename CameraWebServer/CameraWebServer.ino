/**********************************************************************
 * Filename    : 1_Blink.ino
 * Description : Simple LED blink test
 * Author      : Rudy Martin / Next Shift Consulting
 * Project     : Artemis DSAI 2025 Camp
 * Modified    : 2025-05-31
 **********************************************************************/

// 🔧 Choose the onboard LED pin. On many ESP32-S3 boards, it's GPIO 10 or 13.
const int ledPin = 13; // Check your board's spec if this doesn't blink!

void setup() {
  // 🛠️ Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("🚀 Blink test started!");
}

void loop() {
  // 💡 Turn the LED on
  digitalWrite(ledPin, HIGH);
  Serial.println("LED ON");
  delay(500); // Wait for 500ms

  // 💡 Turn the LED off
  digitalWrite(ledPin, LOW);
  Serial.println("LED OFF");
  delay(500); // Wait for 500ms
}

