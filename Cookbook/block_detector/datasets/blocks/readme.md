Below are **ready-to-drop YAML files** that work with Ultralytics YOLOv8.

---

## `datasets/blocks/blocks.yaml`

```yaml
# YOLOv8 data config – “Colored Wooden Blocks” dataset
path: datasets/blocks          # root folder
train: images/train            # images relative to path
val:   images/val

# 12 classes = 6 colours × 2 sizes
names:
  0: red_small
  1: red_large
  2: blue_small
  3: blue_large
  4: green_small
  5: green_large
  6: yellow_small
  7: yellow_large
  8: black_small
  9: black_large
  10: white_small
  11: white_large
```

*Directory reminder*

```
datasets/blocks/
├── images/
│   ├── train/xxx.jpg
│   └── val/yyy.jpg
└── labels/
    ├── train/xxx.txt
    └── val/yyy.txt
```

(Label files follow normal YOLO “class x\_center y\_center w h” format.)

---

## Hyper-parameter sweep presets (`sweeps/`)

These files override defaults when you pass
`cfg='sweeps/fast.yaml'` (or balanced / accuracy).

### `sweeps/fast.yaml`  — **speed first**

```yaml
# Train super-quick; accept lower mAP
epochs: 15
imgsz: 224
batch: 32         # fits on free Colab T4
patience: 3
lr0: 0.01         # higher LR converges fast
momentum: 0.9
hsv_h: 0.015
hsv_s: 0.7
degrees: 0.0      # no rotation aug
translate: 0.0    # no translation aug
scale: 0.5
flipud: 0.0
fliplr: 0.5
```

---

### `sweeps/balanced.yaml`  — **good trade-off**

```yaml
# Default-ish values tuned for coloured blocks
epochs: 30
imgsz: 256
batch: 32
patience: 5
lr0: 0.005
momentum: 0.937
weight_decay: 0.0005
hsv_h: 0.0138
hsv_s: 0.664
degrees: 5.0
scale: 0.5
translate: 0.1
flipud: 0.0
fliplr: 0.5
```

---

### `sweeps/accuracy.yaml`  — **push for highest mAP**

```yaml
# Slower – larger images and heavier aug
epochs: 50
imgsz: 320
batch: 16              # smaller batch to fit GPU RAM
patience: 7
lr0: 0.003
momentum: 0.937
weight_decay: 0.0005
hsv_h: 0.015
hsv_s: 0.7
degrees: 10.0
scale: 0.6
translate: 0.2
mosaic: 1.0            # keep mosaic on
mixup: 0.2
flipud: 0.0
fliplr: 0.5
```

---

### How to use a sweep

```python
model.train(
    data='datasets/blocks/blocks.yaml',
    cfg='sweeps/balanced.yaml',   # or fast / accuracy
    epochs=25                     # can still override
)
```

These four YAML files complete the “model-dev & deploy” resource set for your students.
