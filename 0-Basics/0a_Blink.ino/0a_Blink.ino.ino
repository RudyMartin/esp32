/**********************************************************************
 * Filename    : 0a_Blink.ino
 * Description : Simple LED blink test
 * Author      : Rudy Martin / Next Shift Consulting
 * Project     : Artemis DSAI 2025 Camp
 * Modified    : 2025-05-31
 **********************************************************************/

const int ledPin = 13;  // Adjust based on your ESP32-S3 board

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  delay(200); // Give serial time to wake up

  // 🟢 Friendly welcome message
  Serial.println("\n===============================");
  Serial.println("  💡 Artemis Lab 1: Blink Test");
  Serial.println("  📡 Board is alive and ready!");
  Serial.println("===============================\n");
}

void loop() {
  // 🔆 Turn LED ON
  digitalWrite(ledPin, HIGH);
  Serial.println("✨ LED ON");
  delay(500);

  // 🌑 Turn LED OFF
  digitalWrite(ledPin, LOW);
  Serial.println("💤 LED OFF");
  delay(500);
}

