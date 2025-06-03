/**********************************************************************
 * File       : c_servo_pca9685_buzzer.ino
 * Title      : PCA9685 Servo Auto Detection + Buzzer
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: Sweeps all PCA9685 channels with:
 *              - Channel-specific beep patterns
 *              - Success/failure indicators
 *              - LED sync with buzzer
 * Status.     : Works when connected to 3.3v but no servo movement
 **********************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const uint8_t SERVO_SDA = 6;
const uint8_t SERVO_SCL = 7;
const uint8_t BUZZER_PIN = 14;  // Your buzzer GPIO pin
const uint8_t LED_PIN = 13;     // Onboard or external LED

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// --- Function Declarations ---
void waitForSerial();
void scanI2CDevices(TwoWire &bus);
void testAndLogServosWithBuzzer();
void beepPattern(int buzzerPin, int ledPin, int count, int onTime, int offTime);
void channelBeep(int ch);
void successBeep();
void failureBeep();

void setup() {
  Serial.begin(115200);
  waitForSerial();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Serial.println("🔌 Starting PCA9685 Servo Detection with Buzzer");

  Wire.begin(SERVO_SDA, SERVO_SCL);
  Wire.setClock(400000);

  scanI2CDevices(Wire);

  Serial.println("⚙️  Initializing PCA9685...");
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(100);

  Serial.println("✅ PCA9685 ready. Beginning servo test with buzzer...");
  testAndLogServosWithBuzzer();
}

void loop() {
  // Idle after one full test pass
}

/* ---------------------------------------------
 * Serial wait for boards like ESP32-S3
 * --------------------------------------------- */
void waitForSerial() {
  unsigned long start = millis();
  while (!Serial && millis() - start < 10000) {
    delay(100);
  }
  delay(1000);
  Serial.println("📡 Serial connected. Waiting to stabilize...");
  delay(1000);
}

/* ---------------------------------------------
 * I2C scan with PCA9685 MODE1 signature check
 * --------------------------------------------- */
void scanI2CDevices(TwoWire &bus) {
  Serial.println("🔍 Scanning I2C bus for devices...");

  byte error, address;
  int nDevices = 0;
  bool pcaFound = false;

  for (address = 1; address < 127; address++) {
    bus.beginTransmission(address);
    error = bus.endTransmission();

    if (error == 0) {
      Serial.print("✅ I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);

      bus.beginTransmission(address);
      bus.write(0x00);
      if (bus.endTransmission(false) == 0 && bus.requestFrom(address, (uint8_t)1)) {
        byte mode1 = bus.read();
        Serial.print(" | MODE1: 0x");
        Serial.print(mode1, HEX);

        if (mode1 == 0x00 || mode1 == 0x01) {
          Serial.print(" ➜ Likely PCA9685 ✅");
          pcaFound = true;
        } else {
          Serial.print(" ❓ Unexpected MODE1 value");
        }
      } else {
        Serial.print(" ❌ Could not read MODE1");
      }

      Serial.println();
      nDevices++;
    }
  }

  if (nDevices == 0) {
    Serial.println("❌ No I2C devices found. Check wiring and power.");
  } else {
    Serial.printf("🔢 Total I2C devices found: %d\n", nDevices);
    if (pcaFound) {
      Serial.println("🎯 PCA9685 detected and responsive.");
    } else {
      Serial.println("⚠️ PCA9685 not positively identified.");
    }
  }
}

/* ---------------------------------------------
 * Channel-specific servo test + tone + LED
 * --------------------------------------------- */
void testAndLogServosWithBuzzer() {
  for (int ch = 0; ch < 16; ch++) {
    Serial.printf("\n📦 Testing Channel %02d...\n", ch);

    // Beep pattern varies by channel (1–4)
    channelBeep(ch);

    // Move servo: center → extended → center
    pwm.setPWM(ch, 0, 400);  // center
    delay(400);
    pwm.setPWM(ch, 0, 600);  // extended
    delay(250);
    pwm.setPWM(ch, 0, 400);  // return
    delay(250);
    pwm.setPWM(ch, 0, 0);    // stop

    // Simulated logic: channels 0–7 expected to have servos
    bool expected = ch < 8;

    if (expected) {
      successBeep();
      Serial.printf("✅ Channel %02d expected to move: Success pattern sent\n", ch);
    } else {
      failureBeep();
      Serial.printf("⚠️  Channel %02d not expected to move: Failure pattern sent\n", ch);
    }

    delay(300);  // Small pause between channels
  }

  Serial.println("\n✅ Full sweep complete.");
  Serial.println("📓 Watch LED + sound for success/failure indicators.");
}

/* ---------------------------------------------
 * General beep pattern with LED sync
 * --------------------------------------------- */
void beepPattern(int buzzerPin, int ledPin, int count, int onTime, int offTime) {
  for (int i = 0; i < count; i++) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(onTime);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    delay(offTime);
  }
}

/* ---------------------------------------------
 * Channel beep varies pattern (1–4 beeps)
 * --------------------------------------------- */
void channelBeep(int ch) {
  int beeps = 1 + (ch % 4);  // Pattern 1–4
  beepPattern(BUZZER_PIN, LED_PIN, beeps, 80, 100);
}

/* ---------------------------------------------
 * Success = double short beep
 * --------------------------------------------- */
void successBeep() {
  beepPattern(BUZZER_PIN, LED_PIN, 2, 120, 100);
}

/* ---------------------------------------------
 * Failure = long sad beep
 * --------------------------------------------- */
void failureBeep() {
  beepPattern(BUZZER_PIN, LED_PIN, 1, 400, 300);
}

