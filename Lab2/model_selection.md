

### ✅ Best Roboflow Model Type

**1. Image Classification**

* **Recommended**: Use **Image Classification (Single Label)**.
* Why: You're assigning one label like `"red_tall"` or `"blue_short"` per image. Simple and fast for embedded use.

---

### 🧱 Labeling Strategy for Lab 2

Use **composite labels** combining color and size:

* Examples:
  `red_tall`, `red_short`, `blue_tall`, `green_short`, etc.

Alternatively, if you want flexibility for future FSM decisions:

* Use **two classification models**:

  * One for `color`: red, green, blue
  * One for `size`: short, tall
    (This helps when logic depends on either dimension independently.)

---

### 🧠 Model Training Tips

| Feature      | Suggestion                                        |
| ------------ | ------------------------------------------------- |
| Image Size   | Keep to 96×96 or 128×128 for ESP32 inference      |
| Format       | Use Roboflow’s “TensorFlow Lite” export format    |
| Data Volume  | 30–50 images per class minimum                    |
| Augmentation | Use Roboflow’s auto augment (lighting, rotation)  |
| Backgrounds  | Vary lighting and angle, but keep object centered |

---

### 🖼️ Real-World Teaching Tie-in

> “Students, every time you add a new object class, your FSM stays the same — only your model data updates. That’s the power of modular AI!”

---

