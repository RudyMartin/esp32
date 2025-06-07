Below is a tight, **model-only “cookbook”** for your students.
Everything fits in a single repo folder called **`/block_detector/`** so they can clone-and-go.

---

## 1 🔧 Folder Layout & Resources

| Item                    | Path (inside repo)                                                     | Purpose                            |
| ----------------------- | ---------------------------------------------------------------------- | ---------------------------------- |
| **Colab notebook**      | `Train_YOLOv8_BlockDetector.ipynb`                                     | One-click training / export        |
| **Dataset config**      | `datasets/blocks/blocks.yaml`                                          | Points YOLO to images + labels     |
| **Timing sketch**       | `arduino/ESP32/timing_test.ino`                                        | Measures on-device FPS / ms        |
| **Hyper-param presets** | `sweeps/fast.yaml`<br>`sweeps/balanced.yaml`<br>`sweeps/accuracy.yaml` | Ready-made sweep configs           |
| **Model card template** | `model_card.md`                                                        | Students record size, mAP, latency |
| **Instructions**        | `instructions.md`                                                      | Step-by-step guide below           |

> **Data placement**
> Put training images in `datasets/blocks/images/` and YOLO-format labels in `datasets/blocks/labels/`.
> (The `.yaml` already uses relative paths.)

---

## 2 🚀 High-Level Plan

1. **Train baseline**

   * Open Colab, run all cells in `Train_YOLOv8_BlockDetector.ipynb`.
   * Export both PyTorch (`.pt`) and quantized TFLite (`best_int8.tflite`).

2. **Deploy to ESP32-S3**

   * Flash `timing_test.ino`, copy `best_int8.tflite` to SPIFFS/MicroSD, observe latency.

3. **Sweep & Tune**

   * Pick a preset from `/sweeps/`, retrain via Colab (`--cfg sweeps/fast.yaml` etc.).
   * Re-export → re-flash → measure latency vs accuracy.

4. **Document**

   * Fill out `model_card.md` for the best run.
   * Commit weights + card back to repo.

All MCP/FSM hooks are outside this exercise; students will drop the final `.tflite` into their `CLASSIFY` state later.

---

## 3 📝 `instructions.md` (copy this file verbatim)

````markdown
# Block Detector – Model Development & Deployment Guide
### Lab 3 (Morning + Afternoon)

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

```

---

### ✅ You Now Have

* A **repeatable recipe** students can finish in ~2 hours.
* All artifacts version-controlled for later labs.
* Clean separation: **model dev & deploy only**—integration comes next day.

Ping me if you need the Colab notebook starter code or the Arduino sketch text inline.
```
