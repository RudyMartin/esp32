int outPorts[] = {9, 10, 11, 12};  // IN1–IN4 from ULN2003
int step_number = 0;  // ✅ Add this here

byte stepSequence[4][4] = {
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1}
};
const int NUM_STEPS = 4;

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 Stepper Test Starting...");
  for (int i = 0; i < 4; i++) {
    pinMode(outPorts[i], OUTPUT);
  }
}

int maxLoops = 100;  // 🛑 Safety limit
int loopCounter = 0;

void loop() {
  if (loopCounter >= maxLoops) {
    Serial.println("✅ Safety Stop: Max loops reached.");
    while (true) {
      // 🔒 Stay here forever until reset
      delay(1000);
    }
  }

  Serial.printf("🔁 Loop %d: Rotating CW...\n", loopCounter + 1);
  moveSteps(true, 512*4);  // ~1 full turn
  delay(1000);
  loopCounter++;

  Serial.printf("🔁 Loop %d: Rotating CCW...\n", loopCounter + 1);
  moveSteps(false, 512*4);
  delay(1000);
  loopCounter++;
}


void moveSteps(bool dir, int steps) {
  for (int i = 0; i < steps; i++) {
    setStep(step_number);
    step_number += dir ? 1 : -1;
    if (step_number >= NUM_STEPS) step_number = 0;
    if (step_number < 0) step_number = NUM_STEPS - 1;
    delay(5);  // Try 5–15ms
  }
}


void setStep(int stepIndex) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(outPorts[i], stepSequence[stepIndex][i]);
  }
}
