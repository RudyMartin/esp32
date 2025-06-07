# 0-Basics – Hardware & Library Sanity Checks for Artemis DSAI 2025

Welcome to the Artemis Camp setup zone! This folder contains quick tests to verify your ESP32-S3 gear is working **before** jumping into FSM logic and robot programming.

---

## 🧪 What This Folder Does
Each sketch tests one part of your kit. Run these **in order** to confirm:
- ✅ Your ESP32-S3 board is working
- ✅ Your servo and camera are wired correctly
- ✅ Your Wi-Fi login works
- ✅ Your serial output is formatted right

---

## 📂 Folder Contents

```plaintext
0-Basics/
├── Blink.ino               # Test onboard LED (GPIO 38)
├── ServoTest.ino           # Sweep a servo back and forth
├── CameraTest.ino          # Capture a frame and show image size
├── WiFiTest.ino            # Connect to Wi-Fi and print IP
├── JSONPrintTest.ino       # Format a prediction-like JSON string
└── README.md               # This guide!
```

---

## ✅ Test Overview

| Sketch           | Tests This                     | Expected Output                             |
|------------------|-------------------------------|---------------------------------------------|
| `Blink.ino`      | Board setup, LED              | LED blinks every second                     |
| `ServoTest.ino`  | Servo wiring + movement       | Servo sweeps between 0° and 180°           |
| `CameraTest.ino` | Camera connection + init      | Serial prints image size in bytes           |
| `WiFiTest.ino`   | Wi-Fi SSID/password           | Shows "Connected!" and IP address           |
| `JSONPrintTest.ino` | JSON formatting & serial    | Prints fake classification JSON             |

---

## 🧠 Why This Matters
If your servo isn’t moving or camera fails later, it might be a hardware setup issue — not your FSM code! These tests **save time and frustration**.

You’ll reuse **every skill here** in:
- **Lab 2**: Camera + JSON
- **Lab 3**: Wi-Fi + LED output
- **Capstone**: All of the above

---

## 🚀 Instructions
1. Plug in your ESP32-S3 (USB-C)
2. Select **ESP32S3 Dev Module** from **Tools → Board**
3. Select the correct **COM port**
4. Open a sketch, upload, and open the **Serial Monitor (115200)**
5. Observe output or movement!

If anything fails, ask a TA or compare your wiring.

---

## 🎯 Goal: Green Across the Board!
When all 5 tests pass, you’re officially ready to start **Mission Labs**.

_This folder is your pre-flight checklist._

Good luck, cadet!

– The Artemis DSAI Team
