/**********************************************************************
 * File       : b_blink_test_unl2003_servo.ino
 * Title      : ULN2003 Board LED Blink Test
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp – Hardware Check
 * Description: Blinks the 4 LEDs on the ULN2003 board one at a time
 **********************************************************************/

#define IN1 9
#define IN2 10
#define IN3 11
#define IN4 12

void setup() {
  Serial.begin(115200);
  Serial.println("🔌 ULN2003 LED Test Starting...");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("✅ Pins configured. Blinking LEDs on ULN2003...");
}

void loop() {
  digitalWrite(IN1, HIGH); delay(300);
  digitalWrite(IN1, LOW);

  digitalWrite(IN2, HIGH); delay(300);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH); delay(300);
  digitalWrite(IN3, LOW);

  digitalWrite(IN4, HIGH); delay(300);
  digitalWrite(IN4, LOW);
}