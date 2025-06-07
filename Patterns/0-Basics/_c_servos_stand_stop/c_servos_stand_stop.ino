/**********************************************************************
 * File       : c_servos_start_stop.ino
 * Title      : Modular PCA9685 Servo Controller Shell
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: A reusable button-controlled shell for servo routines.
 **********************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- Pin Configuration ---
const uint8_t SERVO_SDA   = 4;
const uint8_t SERVO_SCL   = 5;
const uint8_t BUZZER_PIN  = 14;
const uint8_t LED_PIN     = 13;
const uint8_t START_BTN   = 12;
const uint8_t STOP_BTN    = 11;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// --- Runtime State ---
bool runMode = false;  // True = execute routine
bool stopFlag = false; // Set when STOP button is pressed

// --- Forward Declarations ---
void waitForSerial();
void scanI2CDevices(TwoWire &bus);
void shellLoop();
void beepOnce();
void runSelectedRoutine();  // <== SWAPPABLE CORE FUNCTION
void runRoutine_sweep();    // Example: default sweep routine
// Future: void runRoutine_classify(), etc.


bool checkStop() {
  if (digitalRead(STOP_BTN) == LOW) {
    Serial.println("⏹️ STOP button pressed mid-pattern.");
    stopFlag = true;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    return true;
  }
  return false;
}

/* ---------------------------------------------
 * Binary tone pattern + servo sync for each bit
 * Each 1-bit = beep + servo move
 * Each 0-bit = pause only
 * Slower for dramatic effect.
 * --------------------------------------------- */
void playBinaryToneAndMove(uint8_t channel, uint8_t bits = 4, int onTime = 200, int offTime = 200) {
  Serial.printf("CH%02d → ", channel);
  for (int i = bits - 1; i >= 0; i--) {
    if (stopFlag || checkStop()) break;

    bool bit = (channel >> i) & 0x01;
    Serial.print(bit ? "1" : "0");

    if (bit) {
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);

      pwm.setPWM(channel, 0, 600);
      for (int t = 0; t < onTime / 2; t += 20) {
        delay(20);
        if (checkStop()) break;
      }

      pwm.setPWM(channel, 0, 400);
      for (int t = 0; t < onTime / 2; t += 20) {
        delay(20);
        if (checkStop()) break;
      }

      pwm.setPWM(channel, 0, 0);
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
    } else {
      for (int t = 0; t < offTime; t += 20) {
        delay(20);
        if (checkStop()) break;
      }
    }

    if (checkStop()) break;
    delay(150);  // dramatic inter-bit pause
  }
  Serial.println();
}


void setup() {
  Serial.begin(115200);
  waitForSerial();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(STOP_BTN, INPUT_PULLUP);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Serial.println("🔌 Starting Modular PCA9685 Servo Shell");

  Wire.begin(SERVO_SDA, SERVO_SCL);
  Wire.setClock(400000);

  scanI2CDevices(Wire);

  Serial.println("⚙️  Initializing PCA9685...");
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(100);

  Serial.println("✅ System ready. Press START to run selected routine.");
}

void loop() {
  shellLoop();  // all control logic here
}

/* ---------------------------------------------
 * Shell Control Loop: handles start/stop
 * --------------------------------------------- */
void shellLoop() {
  static bool waiting = true;

  // START button triggers routine
  if (digitalRead(START_BTN) == LOW && !runMode) {
    Serial.println("▶️ START button pressed. Executing routine...");
    digitalWrite(LED_PIN, HIGH);
    beepOnce();
    runMode = true;
    stopFlag = false;
    runSelectedRoutine();  // 🔁 <-- This is your hot-swappable core!
    digitalWrite(LED_PIN, LOW);
    runMode = false;
    Serial.println("✅ Routine completed or stopped.");
  }

  // STOP button interrupt
  if (digitalRead(STOP_BTN) == LOW && runMode) {
    Serial.println("⏹️ STOP button pressed. Interrupting routine...");
    stopFlag = true;
    beepOnce();
    delay(300);  // debounce
  }

  delay(50);
}

/* ---------------------------------------------
 * SWAPPABLE: Replace this with any routine
 * --------------------------------------------- */
void runSelectedRoutine() {
  runRoutine_sweep();  // Replace with your logic: runRoutine_detect(), etc.
}

/* ---------------------------------------------
 * Example Routine: Sweep all channels (abortable)
 * --------------------------------------------- */
void runRoutine_sweep() {
  for (int ch = 0; ch < 16; ch++) {
    if (stopFlag) break;

    Serial.printf("🔁 Channel %02d: Binary Motion\n", ch);

    // 🧠 Play binary tone+motion pattern
    playBinaryToneAndMove(ch, 4, 200, 200);  // slower, clearer

    delay(300);  // rest between channels
  }

  Serial.println("✅ Full channel routine complete.");
}

/* ---------------------------------------------
 * Optional: I2C device scan
 * --------------------------------------------- */
void scanI2CDevices(TwoWire &bus) {
  Serial.println("🔍 Scanning I2C bus...");
  bool found = false;
  for (uint8_t addr = 1; addr < 127; addr++) {
    bus.beginTransmission(addr);
    if (bus.endTransmission() == 0) {
      Serial.print("✅ Device found at 0x");
      Serial.println(addr, HEX);
      found = true;
    }
  }
  if (!found) Serial.println("⚠️ No I2C devices found.");
}

/* ---------------------------------------------
 * Simple feedback beep
 * --------------------------------------------- */
void beepOnce() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);
}

/* ---------------------------------------------
 * Serial wait for boards like ESP32-S3
 * --------------------------------------------- */
void waitForSerial() {
  unsigned long start = millis();
  while (!Serial && millis() - start < 10000) delay(100);
  delay(500);
  Serial.println("📡 Serial connected.");
}

