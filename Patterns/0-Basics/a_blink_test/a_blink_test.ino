/**********************************************************************
 * Filename    : a_blink_test.ino
 * Description : Simple LED blink test with personalization
 * Author      : Rudy Martin / Next Shift Consulting
 * Project     : Artemis DSAI 2025 Camp
 * Modified    : 2025-05-31
 **********************************************************************/

/*
👋 Welcome to Lab 1 – Blink Test
👉 IMPORTANT: Open the Serial Monitor in Arduino IDE
   Tools → Serial Monitor (or press Ctrl+Shift+M)
   Set Baud Rate to 115200
*/

const int ledPin = 13;  // Adjust if needed for your ESP32-S3 board

// 💡 STUDENT CONFIG – Update this before running!
String username = "YourNameHere";

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  delay(300); // Give serial time to initialize

  // 🕒 Wait 5 seconds before starting
  delay(5000);

  // 🟢 Friendly startup message
  Serial.println("\n===============================");
  Serial.println("  💡 Artemis Lab 1: Blink Test");
  Serial.print("  🧑‍🚀 Hello, "); Serial.print(username); Serial.println("!");
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
