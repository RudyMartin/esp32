/**********************************************************************
 * File       : c_servos_tune.ino
 * Title      : Modular PCA9685 Servo Controller Shell (with MCP Logging)
 * Author     : Rudy Martin / Next Shift Consulting
 * Project    : Artemis DSAI 2025 Camp
 * Description: A reusable shell to test agent routines.
 *              Includes local SD + serial logging.
 *              Stub for WiFi NAS log included (commented).
 **********************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SD.h>
#include <SPI.h>
// #include <WiFi.h>         // Uncomment when ready for NAS logging
// #include <HTTPClient.h>   // Uncomment for HTTP uploads

// --- Pin Configuration ---
const uint8_t SERVO_SDA   = 6;
const uint8_t SERVO_SCL   = 7;
const uint8_t BUZZER_PIN  = 14;
const uint8_t LED_PIN     = 13;

// --- SD Config (change as needed) ---
const int SD_CS_PIN = 10;  // or 5 or 4 depending on board

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// --- Runtime State ---
bool stopFlag = false;

// --- Logging Options ---
bool LOG_TO_SERIAL = true;
bool LOG_TO_SD     = true;
bool LOG_TO_WIFI   = false;  // Enable when NAS ready

// --- MCP Stub: Log wrapper ---
void logEvent(const String& message) {
  if (LOG_TO_SERIAL) Serial.println("[LOG] " + message);

  if (LOG_TO_SD) {
    File logFile = SD.open("/servo_log.txt", FILE_APPEND);
    if (logFile) {
      logFile.println(message);
      logFile.close();
    }
  }

  // if (LOG_TO_WIFI) {
  //   HTTPClient http;
  //   http.begin("http://192.168.1.123/logs/upload");  // NAS IP + endpoint
  //   http.addHeader("Content-Type", "application/json");
  //   String payload = "{\"msg\": \"" + message + "\"}";
  //   int code = http.POST(payload);
  //   http.end();
  // }
}

// --- Routine Placeholder ---
void runSelectedRoutine();  // Hot-swappable routine
void runRoutine_sweep();
void playBinaryToneAndMove(uint8_t channel, uint8_t bits = 4, int onTime = 200, int offTime = 200);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("📡 Serial connected.");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(SERVO_SDA, SERVO_SCL);
  Wire.setClock(400000);

  pwm.begin();
  pwm.setPWMFreq(50);
  delay(100);

  if (LOG_TO_SD) {
    if (!SD.begin(SD_CS_PIN)) {
      Serial.println("⚠️ SD card init failed.");
      LOG_TO_SD = false;
    } else {
      Serial.println("💾 SD card ready.");
    }
  }

  Serial.println("✅ System initialized. Running agent logic...\n");
  runSelectedRoutine();  // Main routine
}

void loop() {
  // Empty — handled in runSelectedRoutine()
}

/* ---------------------------------------------
 * Core Routine Shell (MCP agent entry)
 * --------------------------------------------- */
void runSelectedRoutine() {
  runRoutine_sweep();
}

/* ---------------------------------------------
 * Sweep Routine with Logging
 * --------------------------------------------- */
void runRoutine_sweep() {
  for (int ch = 0; ch < 16; ch++) {
    if (stopFlag) break;
    String msg = "Running channel " + String(ch);
    logEvent(msg);

    Serial.printf("🔁 Channel %02d\n", ch);
    playBinaryToneAndMove(ch);
    delay(300);
  }

  logEvent("✅ Routine finished");
}

/* ---------------------------------------------
 * Patterned Beep + Servo Motion by Binary Value
 * --------------------------------------------- */
void playBinaryToneAndMove(uint8_t channel, uint8_t bits, int onTime, int offTime) {
  Serial.printf("CH%02d → ", channel);
  for (int i = bits - 1; i >= 0; i--) {
    bool bit = (channel >> i) & 0x01;
    Serial.print(bit ? "1" : "0");

    if (bit) {
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      pwm.setPWM(channel, 0, 600);
      delay(onTime / 2);
      pwm.setPWM(channel, 0, 400);
      delay(onTime / 2);
      pwm.setPWM(channel, 0, 0);
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
    } else {
      delay(offTime);
    }

    delay(150);  // Dramatic inter-bit spacing
  }
  Serial.println();
}
