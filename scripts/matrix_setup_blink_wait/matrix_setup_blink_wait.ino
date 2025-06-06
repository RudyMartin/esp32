/********************************************************************
 * matrix_setup_blink_wait.ino
 * Enhanced Pin-by-pin verification for ESP32-S3 HUB75 Matrix connection
 * Tests each pin individually with extended timing and enhanced blinking
 ********************************************************************/
#include <PxMatrix.h>
#include <Ticker.h>

// =====================
// CONFIG & HELPERS
// =====================
struct PinConfig {
  int pin;
  String name;
  String function;
  String ribbonColor;
};

PinConfig config3[] = {
  {42, "R1", "Red Upper", "Blue"},
  {41, "G1", "Green Upper", "Green"},
  {40, "B1", "Blue Upper", "Yellow"},
  {39, "R2", "Red Lower", "Orange"},
  {38, "G2", "Green Lower", "Red"},
  {37, "B2", "Blue Lower", "Brown"},
  {45, "A", "Address A", "Black"},
  {48, "B", "Address B", "White"},
  {47, "C", "Address C", "Silver"},
  {21, "D", "Address D", "Purple"},
  {14, "E", "Address E", "Blue"},
  {-1, "GND", "Ground", "Green"},
  {2, "LAT", "Latch", "Yellow"},
  {1, "OE", "Output Enable", "Orange"},
  {36, "CLK", "Clock", "Red"},
  {-1, "5V", "Power 5V", "Brown"}
};

PinConfig* activeConfig = config3;
const int configSize = sizeof(config3) / sizeof(PinConfig);

// Control signal names (for PxMatrix)
const char* pxControlPins[8] = {"LAT", "OE", "CLK", "A", "B", "C", "D", "E"};
int pxPins[8];  // Will hold resolved GPIOs

// Lookup GPIO number by short pin name
int getPinByName(const String& name) {
  for (int i = 0; i < configSize; i++) {
    if (activeConfig[i].name == name) {
      return activeConfig[i].pin;
    }
  }
  Serial.println("❌ ERROR: Pin not found -> " + name);
  return -1;
}

// Populate pxPins[] based on name list
void initializePxControlPins() {
  for (int i = 0; i < 8; i++) {
    pxPins[i] = getPinByName(pxControlPins[i]);
  }
}

// =====================
// PxMatrix Setup
// =====================
PxMATRIX* display;
Ticker display_ticker;

void display_updater() {
  display->display(70);  // ~14 FPS refresh
}

// =====================
// Setup
// =====================
void setup() {
  Serial.begin(115200);
  delay(3000);

  Serial.println("\n🔧 ESP32-S3 HUB75 64x64 GPIO Test (with PxMatrix)");

  // Resolve PxMatrix control pins from config
  initializePxControlPins();

  // Create and initialize matrix
  display = new PxMATRIX(
    64, 64,
    pxPins[0], pxPins[1], pxPins[2], // LAT, OE, CLK
    pxPins[3], pxPins[4], pxPins[5], // A, B, C
    pxPins[6], pxPins[7]             // D, E
  );

  display->begin(16);               // 1/32 scan panel
  display->setBrightness(100);
  display->clearDisplay();
  display_ticker.attach_ms(70, display_updater);

  // Set testable GPIOs as OUTPUT
  for (int i = 0; i < configSize; i++) {
    if (activeConfig[i].pin != -1) {
      pinMode(activeConfig[i].pin, OUTPUT);
      digitalWrite(activeConfig[i].pin, LOW);
    }
  }

  // Print mapping table
  Serial.println("\nPin Mapping:");
  Serial.println("Idx | GPIO | Name | Function         | Ribbon Color");
  Serial.println("----|------|------|------------------|--------------");
  for (int i = 0; i < configSize; i++) {
    String pinStr = (activeConfig[i].pin == -1) ? "N/A" : String(activeConfig[i].pin);
    Serial.printf("%2d  | %-4s | %-4s | %-16s | %s\n",
      i + 1,
      pinStr.c_str(),
      activeConfig[i].name.c_str(),
      activeConfig[i].function.c_str(),
      activeConfig[i].ribbonColor.c_str()
    );
  }

  delay(2000);
}

// =====================
// Test Logic
// =====================
void matrixBlink(int pin) {
  for (int i = 0; i < 10; i++) {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
  }
}

void enhancedBlink(int pin) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(pin, HIGH);
    delay(3000);
    digitalWrite(pin, LOW);
    delay(2000);
  }
}

void combinedPinTest(int pinIndex) {
  PinConfig p = activeConfig[pinIndex];

  if (p.pin == -1) {
    Serial.printf("⚠️  Skipping Pin %2d (%s - %s): Not testable\n", pinIndex + 1, p.name.c_str(), p.function.c_str());
    delay(1000);
    return;
  }

  Serial.printf("\n🔧 Testing Pin %2d: GPIO %d | %s - %s | Color: %s\n",
    pinIndex + 1, p.pin, p.name.c_str(), p.function.c_str(), p.ribbonColor.c_str());

  // Matrix visual: draw pixel
  display->clearDisplay();
  uint16_t color = display->color565(255, 0, 0);
  display->drawPixel((pinIndex * 4) % 64, (pinIndex * 2) % 64, color);
  display->showBuffer();

  delay(500);
  matrixBlink(p.pin);
  enhancedBlink(p.pin);
  digitalWrite(p.pin, LOW);
  display->clearDisplay();
  display->showBuffer();
  delay(1000);
}

// =====================
// Loop
// =====================
void loop() {
  Serial.println("\n=== Starting Pin Verification Cycle ===");

  for (int i = 0; i < configSize; i++) {
    combinedPinTest(i);
  }

  Serial.println("\n✅ All pins tested. Pausing for review...");
  while (true) {
    delay(60000);
    Serial.println("🔁 Idle. Press RESET to rerun.");
  }
}

