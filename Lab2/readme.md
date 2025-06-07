## 🧠 Lab 2 Focus Recap

### 📸 What Happened:

**Lab 2** introduced **custom model training and deployment**. Students moved from general-purpose LLM use (Lab 1) to training their **own block classifier** using Roboflow.

| Step       | Role in Lab 2                                                             |
| ---------- | ------------------------------------------------------------------------- |
| **Sense**  | Capture labeled images of blocks (color + size)                           |
| **Plan**   | Train a model using Roboflow tools                                        |
| **Act**    | Export and deploy the trained model to a microcontroller (ESP32-S3 or Pi) |
| **Log**    | Print classification results to Serial or screen                          |
| **Repeat** | Test new images and evaluate performance                                  |

---

### 🎯 Key Agent Behavior:

* **Model-powered perception**
  → Students trained a classifier that could recognize **6 colors × 2 sizes** of blocks
  → The system **did not act**, only observed and printed results

---

### 🧰 What Students Built:

* Labeled datasets in Roboflow (from scratch or with pre-collected images)
* Trained a model (classification, not detection)
* Exported the model (TFLite or ONNX)
* Deployed it to a microcontroller and ran inference
* Verified correct predictions via serial/log

---

### 🧠 Learning Goals of Lab 2:

* Understand the **difference between raw data and trained models**
* Practice dataset collection and labeling
* Learn basic model deployment to hardware
* Validate performance and adjust thresholds

---

### ✅ Script Name:

```plaintext
lab2_block_classifier_model_test.ino
```

