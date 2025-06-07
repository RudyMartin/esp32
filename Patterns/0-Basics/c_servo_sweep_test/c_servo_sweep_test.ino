/**********************************************************************
 * File       : servo_sweep_test.ino
 * Title      : Servo Sweep + LED Feedback (100 cycles)
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: Sweeps a servo 100 times with LED feedback, then stops.
 * Status     : Works for one servo at a time
 **********************************************************************/

#include <ESP32Servo.h>

// 🎯 Project: Servo Sweep with LED Feedback
// 🔧 This sketch sweeps a servo back and forth 100 times.
// 💡 An LED on pin 13 blinks during each movement step.
// ✅ After 100 cycles, the LED stays on to signal "All done!"

Servo myservo;

int pos = 0;                  // Servo angle (0–180 degrees)
const int servoPin = 15;      // GPIO pin connected to servo
const int ledPin = 13;        // GPIO pin connected to LED
const int maxCount = 100;     // Number of full back/forth sweeps
int count = 0;                // Counter to track sweeps

void setup() {

// The Servo::attach() function in Arduino (with the ESP32Servo library) can take additional parameters to define min and max pulse widths, 
// which are especially important for small servos like the SG90 9g.

// What does attach(pin, min, max) do?
// pin: The GPIO pin connected to the servo signal wire.
// min: The minimum pulse width in microseconds (typically around 500–600 µs).
// max: The maximum pulse width in microseconds (typically around 2400 µs).



  myservo.attach(servoPin,500,2400);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);  // ✅ Initialize Serial output
  delay(500);            // Optional: give time for Serial Monitor to open

  // 🚀 onStart: Say hello!
  // This is where we prepare the servo and LED for action.
  // The program will now begin moving the servo back and forth.

  Serial.println("🚀 Servo sweep started.");
}

void loop() {
  if (count >= maxCount) {
    Serial.println("✅ Sweep complete. Holding LED on.");
    digitalWrite(ledPin, HIGH);
    while (true);  // Freeze
  }

  Serial.print("🔁 Sweep cycle #");
  Serial.println(count + 1);

    // 🔆 Turn LED ON
  digitalWrite(ledPin, HIGH);
  Serial.println("✨ LED ON");
  delay(500);

  // 🌑 Turn LED OFF
  digitalWrite(ledPin, LOW);
  Serial.println("💤 LED OFF");
  delay(500);

  // 🔁 Forward sweep: 0 to 180 degrees
  for (pos = 0; pos <= 180; pos++) {
    myservo.write(pos);           // Move servo
    digitalWrite(ledPin, HIGH);   // Blink LED on
    delay(10);                    // Wait 10ms
    digitalWrite(ledPin, LOW);    // Blink LED off
  }

  // 🔁 Reverse sweep: 180 to 0 degrees
  for (pos = 180; pos >= 0; pos--) {
    myservo.write(pos);
    digitalWrite(ledPin, HIGH);
    delay(10);
    digitalWrite(ledPin, LOW);
  }

  count++;  // 🔢 One full sweep cycle completed
}

