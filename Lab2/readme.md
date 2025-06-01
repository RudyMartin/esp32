# Lab 2 – Block Classification FSM

**Objective:**
Students will write FSM-based code to capture images from the ESP32-S3 camera, run classification (mock or ML), and transmit results to a network location.

---

## 🔧 Hardware Required
- Freenove ESP32-S3 WROOM CAM board
- USB-C to USB-A cable (for power and serial monitor)
- Colored wood blocks (3–5 types for classification)
- Local Wi-Fi access or instructor-provided NAS endpoint

---

## 🧠 Learning Goals
- Use the ESP32-S3 camera to capture images
- Structure control flow with a modular FSM
- Mock or implement basic classification logic
- Print prediction results
- Upload results to a local network or display via serial

---

## 📂 File Structure
```
2-BlockClassifierFSM/
├── BlockClassifierFSM.ino       # Main FSM code
├── README.md                    # This guide
└── exercises/
    └── lab2_capture_exercises.md
```

---

## ⚙️ FSM States Implemented
| State      | Purpose                                 |
|------------|------------------------------------------|
| `CAPTURE`  | Take picture using onboard camera        |
| `CLASSIFY` | Run mock classifier or inference         |
| `TRANSMIT` | Log result to console or upload to NAS   |
| `WAIT`     | Pause before next round (2 seconds)      |

---

## ✅ Getting Started
1. Open `BlockClassifierFSM.ino` in Arduino IDE
2. Select the correct board: `ESP32S3 Dev Module`
3. Plug in your ESP32-S3 and select the correct COM port
4. Upload the sketch and open the Serial Monitor (115200 baud)

---

## 🚀 Advanced Option (for instructors):
Want to simulate real classification?
- Use a Roboflow-trained `.tflite` model (see Lab 2 Deep Dive)
- Replace the mock `classifyState()` logic with actual inference code
- Or, connect to an external Python server via Wi-Fi to classify remotely

---

## 📌 Tip:
FSM scaffolding here matches later labs, so students can easily add new states (e.g. SAVE_IMAGE, DISPLAY_RESULT) without rewriting structure.

---

## 🧪 Related Labs
- **Lab 1:** Python + chatbot logic (no hardware)
- **Lab 3:** Use a 64x64 LED matrix to display classifications
- **Capstone:** Use this FSM pattern to classify blocks and control robot arm or log mission events
