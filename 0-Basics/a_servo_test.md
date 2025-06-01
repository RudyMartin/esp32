# b_stepper_servo_test.md

## 🚀 Stepper Motor + Servo Testing Guide
Welcome to the ULN2003 + PCA9685 test lab for Artemis DSAI 2025 Camp! Use this to verify wiring, test motion, and begin interactive control using your ESP32-S3 kit.

---

## ✅ Step 1: Blink Test for ULN2003 LEDs
This confirms your **IN1–IN4 pins are correctly connected** and the ULN2003 board is receiving signals.

📌 **We updated the pins to:**
- IN1 → GPIO **9**
- IN2 → GPIO **10**
- IN3 → GPIO **11**
- IN4 → GPIO **12**

---

### 🔧 Sample Sketch: `b_stepper_servo_blink.ino`
```cpp
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
```

---

## 🧪 After Blinking Works
Move on to half-step or full-step motion. Add potentiometer input for speed control, and test direction with `moveSteps()`.

Need help wiring dual power safely? See the diagram above or ask a TA!

---

✅ **Tip:** Always share GND between ESP32, PCA9685, and ULN2003 when using multiple power sources.

Happy debugging and blinking!




| ULN2003 Pin | ESP32-S3 GPIO   | Notes                  |
| ----------- | --------------- | ---------------------- |
| `IN1`       | GPIO **11**     | Step 1 control         |
| `IN2`       | GPIO **12**     | Step 2 control         |
| `IN3`       | GPIO **13**     | Step 3 control         |
| `IN4`       | GPIO **14**     | Step 4 control         |
| `GND`       | GND (any GND)   | Connect to ESP32 GND   |
| `VCC`       | **External 5V** | Use wall adapter only! |


You can get the `Adafruit_PWMServoDriver.h` library from the Arduino Library Manager. Here’s how to install it step by step:

---

### ✅ **Install Adafruit PWM Servo Driver Library**

1. **Open Arduino IDE**
2. Go to **`Sketch` → `Include Library` → `Manage Libraries…`**
3. In the **Library Manager**, type:

   ```
   Adafruit PWM Servo Driver
   ```
4. Look for:

   ```
   Adafruit PWM Servo Driver Library
   Author: Adafruit
   ```
5. Click **Install** (usually installs alongside the required `Adafruit BusIO` dependency).

---

### 🔗 Alternative (Manual Install)

If you prefer GitHub:

* Visit: [https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library)
* Download ZIP
* Then in Arduino IDE:
  **`Sketch` → `Include Library` → `Add .ZIP Library…`** → select the downloaded ZIP

---

✅ 1. #include <Arduino.h>
📦 Built-in with the Arduino core.

✅ No action needed.

✅ 2. #include <Wire.h>
This is the I2C library, also built-in.

✅ No install needed.

✅ 3. #include <Adafruit_PWMServoDriver.h>
❗ YES – You need to install this manually.

📌 Use the Library Manager or install via GitHub.

Search for: “Adafruit PWM Servo Driver”

✅ 4. #include <ESP32Servo.h>
❗ YES – Must install manually.

📌 In Library Manager, search: “ESP32Servo”

Sometimes named: “ESP32Servo by Kevin Harrington
