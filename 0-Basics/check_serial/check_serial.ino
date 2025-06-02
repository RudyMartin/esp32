void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("✅ Serial working!");
}

void loop() {
  Serial.println("📡 Looping...");
  delay(1000);
}
