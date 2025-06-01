# Lab 1 – Blink + Rotate FSM

## 🚀 Welcome to Your First Embedded Mission!

In this lab, you’ll get your Freenove ESP32-S3 board up and running by making it do two simple things:

1. Blink an LED 💡
2. Rotate a connected servo motor 🔁
   ... and you'll do it using **an FSM (Finite State Machine)** — a smart way to organize your code that makes it easier to grow and reuse in later projects.

This is a warm-up lab — a setup and launch pad for **everything that comes next.**

---

## 🔧 What You’ll Need

* Freenove ESP32-S3 WROOM CAM board
* USB-C cable to connect to your laptop
* A small servo (e.g. SG90)
* Jumper wires to connect your servo
* Arduino IDE (installed & working)

> If you're stuck getting the board setup, check the [`0-Setup/`](../0-Setup/) folder!

---

## 🧠 What You’ll Learn

* ✅ How to blink an LED with ESP32
* ✅ How to control a servo using basic FSM logic
* ✅ Why FSMs are awesome for robotics & AI missions
* ✅ How this same pattern will be reused in future labs

---

## 📂 File Overview

```
1-BlinkRotateFSM/
├── BlinkRotateFSM.ino      # Main lab file (upload this!)
├── README.md               # This guide
└── exercises/
    └── lab1_extend_state.md
```

---

## 🕹️ FSM States You’ll Use

| State       | What it does                        |
| ----------- | ----------------------------------- |
| LED\_ON     | Turns the onboard LED on            |
| LED\_OFF    | Turns the LED off                   |
| SERVO\_MOVE | Rotates a servo to a new position   |
| WAIT        | Pauses briefly before looping again |

FSM = A smart controller that steps through **states** like these in a loop.
It’s just like following a recipe: do step A, then B, then C… then back to A.

---

## ✅ How to Run

1. Plug in your ESP32 board via USB
2. Open **Arduino IDE**
3. Load the `BlinkRotateFSM.ino` sketch
4. Select your board: **ESP32S3 Dev Module**
5. Choose the correct **COM port**
6. Hit **Upload** ▶️
7. Open **Serial Monitor** @ 115200 baud to watch output

---

## 💡 Extension Ideas (try in exercises!)

* Add a new **SERVO\_BACK** state to return to the original angle
* Try blinking **faster or slower** (change the `duration`)
* Make the FSM rotate between **3–4 different angles**

See `exercises/lab1_extend_state.md` for guided tasks and extra challenges!

---

## 🔗 How This Lab Connects to Future Work

| Lab      | Builds on this by...                             |
| -------- | ------------------------------------------------ |
| Lab 2    | Replacing LED/servo with camera + classification |
| Lab 3    | Outputting results to LED matrix                 |
| Lab 4    | Adding real **sensor-based decisions**           |
| Capstone | Using FSM to control a real robot mission!       |

> Mastering FSMs now means you’ll be ready to build a **smart robot** by the end of camp. 🧠🤖

---

## 🔍 Troubleshooting

* 💥 Servo not moving? Check your power — some servos need external power (not just USB).
* 🕵️ LED not blinking? Make sure you're using the **onboard LED pin (usually 15 or 2)**.
* ⚠️ “Board not found”? Double-check your **Board & Port** settings under Tools.
r.
