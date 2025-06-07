
# 🧱 Block Detector

Train → Quantize → Benchmark  
A lightning-fast workflow for building a YOLOv8 model that recognizes coloured wooden blocks and measuring its latency on an ESP32-S3.

[![Open in Colab](https://colab.research.google.com/assets/colab-badge.svg)](Train_YOLOv8_BlockDetector.ipynb)
![License: CC0-1.0](https://img.shields.io/badge/License-CC0--1.0-lightgrey.svg)

---

## 📂 What’s Inside?

| Path | Why it exists |
|------|---------------|
| `Train_YOLOv8_BlockDetector.ipynb` | One-click Colab notebook: train, export, quantize |
| `instructions.md` | Step-by-step cookbook for the entire lab |
| `datasets/blocks/blocks.yaml` | YOLO data config (12 classes, 6 colours × 2 sizes) |
| `datasets/blocks/images/` & `labels/` | Starter images & YOLO label files |
| `sweeps/fast.yaml`<br>`sweeps/balanced.yaml`<br>`sweeps/accuracy.yaml` | Hyper-parameter presets: speed / balanced / accuracy push |
| `arduino/ESP32/timing_test.ino` | Sketch that loads the quantized model and prints inference latency |
| `model_card.md` | Template for recording size, accuracy, latency, training notes |

---

## 🏃‍♂️ TL;DR – 3 Steps

```bash
git clone https://github.com/<org>/block_detector.git
cd block_detector

# 1️⃣  Train & export in Colab
#     (open the notebook link above and run Runtime → Run all)
#     Download best_int8.tflite

# 2️⃣  Flash the benchmark sketch
arduino/ESP32/timing_test.ino   # open in Arduino IDE → Upload

# 3️⃣  Copy the model to /model/ on SPIFFS (or SD), hit RESET
#     Read latency numbers in Serial Monitor
````

---

## 🔧 Folder Layout

```
block_detector/
├── README.md
├── instructions.md
├── Train_YOLOv8_BlockDetector.ipynb
├── model_card.md
│
├── datasets/
│   └── blocks/
│       ├── blocks.yaml
│       ├── images/
│       │   ├── train/
│       │   └── val/
│       └── labels/
│           ├── train/
│           └── val/
│
├── sweeps/
│   ├── fast.yaml
│   ├── balanced.yaml
│   └── accuracy.yaml
│
└── arduino/
    └── ESP32/
        └── timing_test.ino
```

> **Generated Artifacts**
> All training outputs land in `runs/…` (auto-created by Ultralytics).
> **Do not commit** those large folders—only push your final `best_int8.tflite` (if desired) and an updated `model_card.md`.

---

## 🗺️ Learning Roadmap

| Stage                 | Goal                                        | Outcome                                                   |
| --------------------- | ------------------------------------------- | --------------------------------------------------------- |
| **Learning Day 2**             | Use a pre-trained model                     | Immediate success, connects to FSM `CLASSIFY` state       |
| **Learning Day 3 – AM**   | **You** train your own YOLOv8-nano          | `.pt` and quantized `.tflite` weights in under 10 minutes |
| **Learning Day 3 – PM** | Tune for speed vs accuracy                  | Measure latency, document trade-offs in `model_card.md`   |
| **Next Labs**         | Integrate tuned model into MCP/FSM pipeline | Autonomous block sorter / mission agent                   |

Happy training & benchmarking!
— Artemis DSAI 2025 Camp Team


