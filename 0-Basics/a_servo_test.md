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

Let me know if you'd like a test-only `.ino` that confirms the PCA9685 is connected and responsive before mixing in servos or steppers.
