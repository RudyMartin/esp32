# Lab 2 – Block Classification FSM

## Objective
Students will write FSM-based code to capture images from the ESP32-S3 camera, run classification (mock or ML), and transmit results to a network location.

---

🔧 **Hardware Required**
- Freenove ESP32-S3 WROOM CAM board
- USB-C to USB-A cable (for power and serial monitor)
- Colored wood blocks (3–5 types for classification)
- Local Wi-Fi access or instructor-provided NAS endpoint

🧠 **Learning Goals**
- Use the ESP32-S3 camera to capture images
- Structure control flow with a modular FSM (Finite State Machine)
- Implement or simulate classification using ML models
- Print and format JSON prediction results
- Upload results to a network location or observe via Serial Monitor

💡 _FSMs help students structure code clearly into phases or actions, making it easier to extend or debug over time._

📂 **File Structure**
```
2-BlockClassifierFSM/
├── BlockClassifierFSM.ino       # Main FSM code with classification logic
├── config_template.h            # Team-specific configuration struct for metadata and Wi-Fi
├── README.md                    # This guide
└── exercises/
    └── lab2_capture_exercises.md
```

⚙️ **FSM States Implemented**
| State     | Purpose                                                  |
|-----------|----------------------------------------------------------|
| CAPTURE   | Capture a photo using the onboard camera module         |
| CLASSIFY  | Run ML model or fallback random label prediction        |
| TRANSMIT  | Create a JSON result and print or send to NAS           |
| WAIT      | Short pause between classification loops (2 seconds)    |

✅ **Getting Started**
1. Open `BlockClassifierFSM.ino` in the Arduino IDE
2. Select board: **ESP32S3 Dev Module** under Tools → Board
3. Connect your ESP32-S3 and select the appropriate **COM port**
4. Upload the sketch and open **Serial Monitor** set to **115200 baud**

🔁 **FSM Loop Summary**
1. **CAPTURE** – Take a snapshot with the ESP32 camera
2. **CLASSIFY** – Apply ML model or pick label from fallback
3. **TRANSMIT** – Create a JSON string and upload or print result
4. **WAIT** – Brief delay before returning to CAPTURE state

🚀 **Advanced Option (for instructors)** _(included)_
Want to simulate real classification?
- Train a model in Roboflow and export as `.tflite`
- Convert the model to a `.h` file using:
  👉 [TFLite to `model_data.h` Converter Script](https://github.com/NextShiftAI/tflite-header-converter)
- Replace fallback logic in `classifyState()` with actual inference _(included)_
- Future option: Wi-Fi-based inference using external server _(not included)_

📦 **Expected JSON Output Format** _(included)_
During the TRANSMIT state, a result like this is generated:
```json
{
  "source": "group_1",
  "sensor": "camera_b",
  "model": "MobileNetV2",
  "label": "red block",
  "confidence": 0.82,
  "timestamp": 1717271938
}
```
➡️ Use "model": "default" if the fallback (mock) logic is used.

📌 **Tip**
The FSM structure is reused in later labs, including Lab 3 (LED matrix) and the Capstone. Once mastered, you’ll be able to add states like `SAVE_IMAGE`, `DISPLAY_RESULT`, or `UPLOAD_LOG` with minimal extra code.

🧪 **Related Labs**
- **Lab 1**: Blinking LEDs and FSM basics
- **Lab 3**: Use a 64x64 LED matrix to display predictions
- **Capstone**: Build a full pipeline that logs object metadata and routes blocks based on mission goals

