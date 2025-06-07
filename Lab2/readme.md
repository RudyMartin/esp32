## 🔬 **Lab 2 – Block Classification FSM**

### 🎯 Objective

Students will write FSM-based code to:

* Capture images from the ESP32-S3 camera
* Run classification (mock or ML-based)
* Format predictions as JSON
* Transmit results to a local NAS or display via Serial Monitor

---

### 🔧 Hardware Required

* ✅ Freenove ESP32-S3 WROOM CAM board
* ✅ USB-C to USB-A cable (power and data)
* ✅ Colored wooden blocks (3–5 types; different **colors and sizes**)
* ✅ Instructor-provided NAS endpoint or local Wi-Fi network

---

### 🧠 Learning Goals

* Use ESP32-S3 to capture images
* Understand FSM (Finite State Machine) as a design pattern
* Simulate or deploy a trained ML model for classification
* Generate and upload/print structured JSON results
* Prep for Lab 3 (LED matrix) and Capstone integration

---

### 💡 Why FSMs?

FSMs organize code into clear **phases** (e.g., CAPTURE → CLASSIFY → TRANSMIT), making logic easier to understand, extend, and debug.

---

### 📂 File Structure

```
2.3-BlockClassifierFSM/
├── 2.3-BlockClassifierFSM.ino      # Main FSM logic
├── config_template.h               # Wi-Fi and metadata
├── model_data.h                    # Optional: real ML model
├── README.md                       # Lab guide
└── exercises/
    └── lab2_capture_exercises.md
```

---

### ⚙️ FSM States Implemented

| State    | Purpose                                   |
| -------- | ----------------------------------------- |
| CAPTURE  | Capture photo from onboard camera         |
| CLASSIFY | Use fallback (mock) or real ML classifier |
| TRANSMIT | Create and print/upload JSON prediction   |
| WAIT     | Pause briefly before restarting loop      |

---

### ✅ Getting Started

1. Open `2.3-BlockClassifierFSM.ino` in Arduino IDE
2. Select board: `ESP32S3 Dev Module` (Tools → Board)
3. Connect via USB and choose correct COM port
4. Upload and run. Open Serial Monitor at **115200 baud**

---

### 🔁 FSM Loop Summary

```
CAPTURE   →  Take snapshot
CLASSIFY  →  Predict label
TRANSMIT  →  Send/print result
WAIT      →  Delay 2 seconds
```

---

### 🚀 **Advanced Option: Real ML Inference (Instructor or Advanced Students)**

#### 🧠 Use Roboflow to Train & Deploy a Model

1. **Capture images**: Include centered blocks of varying colors/sizes
2. **Label images** in Roboflow with a **single class** like `red_small`
3. **Train** using classification mode (e.g., `MobileNetV2`)
4. **Export as**: `TensorFlow Lite (int8 Quantized)`
5. Use provided script to convert `.tflite` → `model_data.h`
6. Include `model_data.h` and update `classifyState()` to use real model inference

---

### 🧪 JSON Output Format (TRANSMIT Phase)

```json
{
  "source": "group_1",
  "sensor": "camera_b",
  "model": "MobileNetV2",
  "label": "red_small",
  "confidence": 0.82,
  "timestamp": 1717271938
}
```

Use `"model": "default"` if fallback/random logic is used instead of inference.

---

### 📌 Tips

* Combine color + size as a **single label**: `red_small`, `blue_large`, etc.
* Save this structure — it’s reused in Lab 3 (LED display) and the Capstone
* Want position mapping (e.g., R1S1)? Add it to the JSON for future labs.

---

### 🧪 Related Labs

* **Lab 1**: Blinking LEDs and FSM basics
* **Lab 3**: Display classification results on a 64×64 LED Matrix
* **Capstone**: Route objects based on prediction + environment

---

