
## 🚀 Stepper Motor Testing Guide
Welcome to the ULN2003 test lab for Artemis DSAI 2025 Camp! Use this to verify wiring, test motion, and begin interactive control using your ESP32-S3 kit.

---

## ✅ Steps to Complete:

### 1: Connect the ULN2003

Connect ONLY the ULN2003 board to the ESP32 breadboard.

Confirm your **IN1–IN4 pins are correctly connected** and the ULN2003 board is receiving signals.

📌 **We updated the pins to:**
UNL2003 -> ESP32 S3
- IN1 → GPIO **9**
- IN2 → GPIO **10**
- IN3 → GPIO **11**
- IN4 → GPIO **12**
- (GND) → GND **(lower left side of the board marked GND where there are 8 pins to choose from)**
- (VCC "Positive") → 5V **(Insert wire into 5V row left side where the blue LED is)**

This is what Freenove shows in its tutorials, but you'll soon see a safer way to power the board.

Anyway, check the order of the lines. It's easy to mix up the wires.

### ⚡ USB Power Test Insight
Before connecting an external power supply:
- Connect **both 5V and GND** of the ULN2003 to the ESP32 board.
- This uses USB power only.

🎓 **Why do this?**
This lets you observe the ULN2003 behavior **underpowered** by USB:
- A stepper motor may vibrate or hum but **won’t rotate reliably**
- It helps you test for shared ground issues
- Most importantly it shows, when you disconnect the USB and the motor keeps running → ⚠️ wiring issue (power leakage or missing common ground)

🚗 Later, you'll switch to a **real power supply** and confirm that the motor stops when that is turned off.

---

### 2: Copy and upload this Blink test code.

### 🔧 Sample Sketch: `b_blink_test_unl2003_servo.ino`
📂 **Save this file as:** `/0-Basics/b_blink_test_unl2003_servo.ino`

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

### 3. Observe Serial Monitor  
Open Serial Monitor at **115200 baud** and confirm status messages:
- "ULN2003 LED Test Starting..."
- "✅ Pins configured..."

### 4. Verify ULN2003 LED response  
If no lights, recheck your wiring!

---

### 5. Run Stepper Motor Script  
Attach the stepper motor to the ULN2003 board  
Swap in the `b_stepper_motor_test.ino` sketch.  
Confirm:
- Full forward and reverse rotation
- Serial monitor logs: "🔁 Rotating CW..." / "🔁 Rotating CCW..."
- Adjust the script so that the motor makes a full turn

---

### 6. Use a Safe Power Supply Schema
- Turn everything off
- Disconnect the 5V power supply going into the ESP32 board 
- Disconnect the 5V wire from ESP32 to ULN2003
- Add an adapter to the 5V power supply:
  - Connect the **+** to the **+ terminal** on the ULN2003
  - Connect the **- (GND)** to the **Shared GND** (lower right side of board)
- NOW turn on and run the script. It should behave the same.
- Then turn **off** the external 5V power and confirm the motor stops — no current leaks!

---

### 7. (Optional) Add a potentiometer for speed control
Use the following wiring:
- One outer leg → GND
- Middle leg → GPIO **34** (analog input)
- Other outer leg → 3.3V

Add this code to use it:
```cpp
int potValue = analogRead(34);
int delayTime = map(potValue, 0, 4095, 2, 20);
delay(delayTime);
```

---

## ✅ Success Checklist
Use this checklist to confirm your setup is complete:

- [ ] Downloaded all **required libraries** (if applicable)
- [ ] Edited **username** in code
- [ ] Uploaded the test sketch successfully
- [ ] ULN2003 board LEDs **blink in sequence**
- [ ] Stepper motor **rotates full 360°** (512 * 4 steps)
- [ ] Turning off **external 5V supply** stops the motor - confirms correct wiring

🧠 Optional Next Step (Advanced):
- [ ] Add a **potentiometer** to control motor speed interactively (recommended for Lab 4 or later)

Happy debugging and blinking!
