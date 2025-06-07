
# Block Detector – Model Development & Deployment Guide
### Day 3 Learning Session (Morning + Afternoon)

---

## Prerequisites
* Google account (for Colab)
* Git & GitHub account
* ESP32-S3 board with camera
* Arduino IDE with board package + TinyML library
* Dataset already copied to `datasets/blocks/`

---

## 1. Clone the Repo

```bash
git clone https://github.com/<org-or-user>/block_detector.git
cd block_detector
````

---

## 2. Train Baseline Model (Colab)

1. Open **Train\_YOLOv8\_BlockDetector.ipynb** in Colab.
2. Run **Runtime → Run all**.
   *Expected time ≈ 7 min on free T4 GPU.*

Outputs:

* `runs/detect/train/weights/best.pt`
* `runs/detect/train/weights/best_int8.tflite` (quantized)

Download both files.

---

## 3. Flash & Benchmark on ESP32-S3

1. Open **arduino/ESP32/timing\_test.ino** in Arduino IDE.
2. Set the correct port & board, then **Upload**.
3. After upload, open Serial Monitor @ 115 200 baud.
4. Copy `best_int8.tflite` to the board (SPIFFS or SD).
5. Press **RESET** – the sketch reports **ms / inference** every frame.

Record the latency number.

---

## 4. Hyper-Parameter Sweep (Optional)

Pick **one** YAML:

| Preset                 | Purpose                             |
| ---------------------- | ----------------------------------- |
| `sweeps/fast.yaml`     | Finishes in \~3 min, lower accuracy |
| `sweeps/balanced.yaml` | Good trade-off                      |
| `sweeps/accuracy.yaml` | Slower, best mAP                    |

In Colab, change the training cell:

```python
model.train(data='datasets/blocks/blocks.yaml',
            cfg='sweeps/balanced.yaml',
            epochs=25)
```

Re-export, flash, and measure latency again.

---

## 5. Document Your Best Model

Edit **model\_card.md**:

```markdown
## Block Detector – Student Team A
| Metric | Value |
|--------|-------|
| Model file | best_int8.tflite |
| Size on disk | 1.3 MB |
| mAP@0.5 | 0.91 |
| Latency (ms) | 34 |
| Training preset | balanced |
| Notes | ---
```

---

## 6. Commit & Push

```bash
git add runs/ model_card.md
git commit -m "Add tuned block detector v1"
git push
```

Congrats! Your custom model is ready for the FSM `CLASSIFY` state.


---

### ✅ You Now Have

* A **repeatable recipe** students can finish in ~2 hours.
* All artifacts version-controlled for later labs.
* Clean separation: **model dev & deploy only**—integration that comes next day.

```
