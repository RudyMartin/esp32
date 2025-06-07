Below are **copy-paste-ready** snippets for both resources the Jupyter Notebook that creates the model and the Arduino the deploys it.

---

## 1️⃣ **`Train_YOLOv8_BlockDetector.ipynb` – Colab Starter**

Create a new Colab notebook and paste the **markdown headings** and **code cells** exactly as shown.

<details>
<summary>Click to view notebook contents</summary>

### **(Markdown Cell)** Title

```markdown
# 🧱 Block Detector – YOLOv8 Training Notebook
Train ➜ Export ➜ Quantize ➜ Download
```

### **(Code Cell #1) – Install & imports**

```python
!pip install ultralytics==0.6.3 --quiet   # stable at time of camp
from ultralytics import YOLO
import shutil, os, zipfile, json, datetime
```

### **(Markdown Cell)** Dataset Mount

```markdown
## 1. Mount /datasets folder
Assumes the repo structure:
```

```
block_detector/
├── datasets/
│   └── blocks/
│       ├── images/   (train/val sub-folders)
│       └── labels/   (YOLO .txt files)
└── blocks.yaml
```

### **(Code Cell #2) – Path sanity check**

```python
DATA_CFG = 'datasets/blocks/blocks.yaml'   # adjust if needed
assert os.path.exists(DATA_CFG), f"{DATA_CFG} not found!"

# Quick peek at class names
with open(DATA_CFG) as f:
    print(f.read())
```

### **(Markdown Cell)** Train baseline

```markdown
## 2. Train Baseline Model (YOLOv8-Nano, 25 epochs)
```

### **(Code Cell #3) – Train**

```python
model = YOLO('yolov8n.pt')       # nano backbone
results = model.train(
    data=DATA_CFG,
    epochs=25,
    imgsz=256,
    patience=5,        # early stop
    project='runs', name='baseline'
)
```

### **(Markdown Cell)** Quantize & export

```markdown
## 3. Export to TFLite (float & int8)
```

### **(Code Cell #4) – Export**

```python
# Pick best.pt
best_pt = 'runs/detect/baseline/weights/best.pt'
assert os.path.isfile(best_pt)

# Float16 TFLite
model.export(format='tflite', weights=best_pt)

# Int8 quantization
model.export(format='tflite', quantize=True, weights=best_pt)
```

### **(Markdown Cell)** Download section

```markdown
## 4. Download Files
Click the folder icon in the left sidebar → `runs/detect/baseline/weights/`  
Download:
* `best.pt`
* `best.tflite`
* `best_int8.tflite`
```

### **(Markdown Cell)** (OPTIONAL) Hyper-param sweep sample

```markdown
## 5. Sweep Example
Change `cfg='sweeps/balanced.yaml'` and rerun the training cell.
```

</details>

---

## 2️⃣ **`timing_test.ino` – ESP32-S3 Latency Sketch**

> **Prereqs**
>
> * Board package: “ESP32 by Espressif” ≥ 3.0.0
> * Libraries: **TensorFlowLite\_ESP32** (TinyML), **SPIFFS** or **SD\_MMC** (choose one)
> * Copy `best_int8.tflite` to `/model/` on SPIFFS **or** to microSD root.

```cpp
/**********************************************************************
 * File       : timing_test.ino
 * Purpose    : Measure inference latency of a TFLite-Micro model
 * Board      : ESP32-S3 (Freenove, XIAO, etc.)
 * Camera     : OV2640 or OV5640 (configure pins below)
 **********************************************************************/

#include "esp_camera.h"
#include "FS.h"
#include "SPIFFS.h"          // or include SD_MMC.h
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_time.h"

// === Camera pins (example for Freenove S3) ===
#define CAM_PIN_PWDN   -1
#define CAM_PIN_RESET  -1
#define CAM_PIN_XCLK   15
#define CAM_PIN_SIOD   4
#define CAM_PIN_SIOC   5
#define CAM_PIN_D7     16
#define CAM_PIN_D6     17
#define CAM_PIN_D5     18
#define CAM_PIN_D4     12
#define CAM_PIN_D3     10
#define CAM_PIN_D2      8
#define CAM_PIN_D1      9
#define CAM_PIN_D0     11
#define CAM_PIN_VSYNC   6
#define CAM_PIN_HREF    7
#define CAM_PIN_PCLK   13

// === TFLite buffer ===
constexpr int kTensorArenaSize = 200 * 1024;     // adjust if model large
static uint8_t tensor_arena[kTensorArenaSize];

tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input  = nullptr;
TfLiteTensor* output = nullptr;

// ------------------------------------------------------------------
// Helper: Load .tflite file into memory
// ------------------------------------------------------------------
const unsigned char* model_data   = nullptr;
size_t               model_length = 0;

bool loadModel(fs::FS& fs, const char* path) {
  File f = fs.open(path, "rb");
  if (!f) { Serial.println("❌ Model file open failed"); return false; }
  model_length = f.size();
  model_data   = (const unsigned char*)malloc(model_length);
  if (!model_data) { Serial.println("❌ malloc failed"); return false; }
  f.read((uint8_t*)model_data, model_length);
  f.close();
  Serial.printf("✅ Model loaded (%u bytes)\n", model_length);
  return true;
}

// ------------------------------------------------------------------
// Arduino setup()
// ------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  // Mount filesystem
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS mount failed");
    return;
  }

  // Load model
  if (!loadModel(SPIFFS, "/model/best_int8.tflite")) return;

  // Create TFLite objects
  auto* model = tflite::GetModel(model_data);
  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, nullptr);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("AllocateTensors failed");
    return;
  }

  input  = interpreter->input(0);
  output = interpreter->output(0);

  // Initialise camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = CAM_PIN_D0;
  config.pin_d1       = CAM_PIN_D1;
  config.pin_d2       = CAM_PIN_D2;
  config.pin_d3       = CAM_PIN_D3;
  config.pin_d4       = CAM_PIN_D4;
  config.pin_d5       = CAM_PIN_D5;
  config.pin_d6       = CAM_PIN_D6;
  config.pin_d7       = CAM_PIN_D7;
  config.pin_xclk     = CAM_PIN_XCLK;
  config.pin_pclk     = CAM_PIN_PCLK;
  config.pin_vsync    = CAM_PIN_VSYNC;
  config.pin_href     = CAM_PIN_HREF;
  config.pin_sccb_sda = CAM_PIN_SIOD;
  config.pin_sccb_scl = CAM_PIN_SIOC;
  config.pin_pwdn     = CAM_PIN_PWDN;
  config.pin_reset    = CAM_PIN_RESET;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size   = FRAMESIZE_96X96;   // match model input
  config.fb_count     = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  Serial.println("\n=== Inference Latency Test ===");
}

// ------------------------------------------------------------------
// Arduino loop()
// ------------------------------------------------------------------
void loop() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Resize (skip if FRAMESIZE matches model)
  memcpy(input->data.uint8, fb->buf, input->bytes);

  uint32_t t0 = micros();
  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Invoke failed");
    return;
  }
  uint32_t dt_us = micros() - t0;

  // Example: assuming output[0] is confidence of class 0
  float conf = output->data.f[0];

  Serial.printf("Latency: %.2f ms | Conf: %.2f\n", dt_us / 1000.0, conf);

  esp_camera_fb_return(fb);
  delay(50);  // ↔ ~20 fps upper bound
}
```

**Compile tips**

* **Menu Config ➜ Partition Scheme**: set ≥ 3 MB SPIFFS (“Minimal SPIFFS, 2.5MB app, 3MB SPIFFS” works great).
* If the model overflows `tensor_arena`, increase `kTensorArenaSize` or pick a smaller (`yolov8n-int8`) file.

---

### Next Steps

1. Save both snippets to your repo (`/block_detector/`).
2. Students open Colab, train, download `best_int8.tflite`.
3. Flash the sketch, drop model into `/model/` on SPIFFS, reset board — latency numbers print every frame.

That’s everything needed to **develop, export, and benchmark** custom models purely within the “model dev & deploy” scope.
