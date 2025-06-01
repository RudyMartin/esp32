# Lab 2 Student Exercises – Image Capture + Classification

These exercises guide you through exploring and modifying your FSM to capture images, classify blocks, and log the results.

---

## 🔍 Exercise 1: Understand the FSM Flow
**Goal:** Trace what each FSM state does

🧩 In your code, identify:
- What happens in `CAPTURE`?
- What is simulated in `CLASSIFY`?
- Where does the `TRANSMIT` result go?

✍️ Write a short comment above each state describing what it should eventually do.

---

## 🎯 Exercise 2: Integrate Your Actual TFLite Classifier (With Fallback)
**Goal:** Replace the mock classifier with actual model-based classification and provide a fallback if the model fails

🔁 In `classifyState()`, try the TFLite model first. If unavailable, use a random fallback.
```cpp
String labels[] = {"red_tall", "blue_short", "green_round", "yellow_square", "purple_triangle"};
int num_classes = sizeof(labels) / sizeof(labels[0]);

String label = "unknown";
float confidence = 0.0;
bool usedFallback = false;

if (interpreter != nullptr) {
  float* scores = interpreter->typed_output_tensor<float>(0);
  int predictedIndex = 0;
  float maxProb = scores[0];

  for (int i = 1; i < num_classes; i++) {
    if (scores[i] > maxProb) {
      maxProb = scores[i];
      predictedIndex = i;
    }
  }
  label = labels[predictedIndex];
  confidence = maxProb;
  Serial.printf("[CLASSIFY] %s (%.2f)\n", label.c_str(), confidence);
} else {
  usedFallback = true;
  predictedIndex = random(0, num_classes);
  label = labels[predictedIndex];
  confidence = random(70, 100) / 100.0; // Simulate 0.70–0.99
  Serial.printf("[FALLBACK] %s (%.2f)\n", label.c_str(), confidence);
}
```
✅ Save these values for Exercise 3.

---

## 🌐 Exercise 3: Format Your Result as JSON
**Goal:** Make your result ready for upload

✏️ In `transmitState()`, replace the message with a real JSON-style log including class, probability, and metadata:
```cpp
#include "config.h"

unsigned long timestamp = millis();
String model_used = usedFallback ? "default" : config.model;

String result = "{\"label\": \"" + label + "\"," +
                 " \"confidence\": " + String(confidence, 2) + "," +
                 " \"timestamp\": " + String(timestamp) + "," +
                 " \"source\": \"" + config.source + "\"," +
                 " \"sensor\": \"" + config.sensor + "\"," +
                 " \"model\": \"" + model_used + "\" }";
Serial.println(result);
```

---

## 💾 Exercise 4 (Bonus): Save to SD Card (Instructor-led)
If your ESP32 board has a MicroSD slot, try writing the JSON result to a `.log` file on SD.

```cpp
File log = SD.open("results.log", FILE_APPEND);
if (log) {
  log.println(result);
  log.close();
}
```

---

## 🧠 Exercise 5: Centralize Your Team’s Metadata Using a Config Struct
**Goal:** Make it easier to manage group info across all states

🔧 Create a `config.h` file with this content:
```cpp
#ifndef CONFIG_H
#define CONFIG_H

struct SystemConfig {
  const char* source = "group_1";
  const char* sensor = "camera_b";
  const char* model = "MobileNetV2";
};

static const SystemConfig config;

#endif
```

📥 Then in your `.ino` file:
```cpp
#include "config.h"
```
Use `config.source`, `config.sensor`, and `config.model` whenever you build output logs or messages.

🧠 This will make it easier for different teams to just change one file to reflect their setup!

---

## 🛠️ Bonus: Convert Roboflow `.tflite` Model to Arduino Header

Use this helper script to convert your model into a C-style `.h` file for use in Arduino:

📄 `convert_tflite_to_header.sh`
```bash
#!/bin/bash
TFLITE_MODEL="model.tflite"
OUTPUT_HEADER="model_data.h"
VARIABLE_NAME="model_tflite"

if [ ! -f "$TFLITE_MODEL" ]; then
  echo "❌ Error: $TFLITE_MODEL not found!"
  exit 1
fi

xxd -i "$TFLITE_MODEL" > "$OUTPUT_HEADER"

# Rename variables for Arduino
sed -i "" "s/unsigned char .*_tflite/const unsigned char ${VARIABLE_NAME}/g" "$OUTPUT_HEADER"
sed -i "" "s/unsigned int .*_tflite_len/const int ${VARIABLE_NAME}_len/g" "$OUTPUT_HEADER"

echo "✅ Done. You can now include: #include \"$OUTPUT_HEADER\""
```

To run:
```bash
chmod +x convert_tflite_to_header.sh
./convert_tflite_to_header.sh
```

---

## 🧠 Reflection
- Why is FSM helpful here?
- What might a 5-state version of this lab look like?
- How could we later add LED signals or servo movement?

