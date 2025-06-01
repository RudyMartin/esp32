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

## 🎯 Exercise 2: Change the Mock Classifier
**Goal:** Improve or personalize the classification output

🔁 In `classifyState()`:
- Replace the mock line with one of your own:
```cpp
Serial.println("[CLASSIFY] Predicted: red tall block");
```
💡 Try using a randomizer:
```cpp
String labels[] = {"red", "blue", "green"};
String sizes[] = {"short", "tall"};
Serial.printf("[CLASSIFY] Predicted: %s %s block\n", labels[random(0,3)].c_str(), sizes[random(0,2)].c_str());
```

---

## 🌐 Exercise 3: Format Your Result as JSON
**Goal:** Make your result ready for upload

✏️ In `transmitState()`, replace the message with a real JSON-style log:
```cpp
String result = "{\"color\": \"red\", \"size\": \"tall\", \"timestamp\": 123456}";
Serial.println(result);
```
You can use `millis()` or `esp_timer_get_time()` for timestamps.

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

## 🧠 Reflection
- Why is FSM helpful here?
- What might a 5-state version of this lab look like?
- How could we later add LED signals or servo movement?
