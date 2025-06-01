TODO: REFRESH THIS TO REFLEACT ACTUAL LABS __> Use of **one of 6 key FSM-driven patterns** in your DSAI-MCP-Camp system.

---

## 🎯 The 6 Lab FSM Patterns Overview

| Lab | `.ino` File           | FSM Type          | Teaches                         | Key Features                  |
| --- | --------------------- | ----------------- | ------------------------------- | ----------------------------- |
| 1   | `BlinkRotateFSM.ino`  | Time-Based FSM    | States, `millis()`, I/O         | LED, timers                   |
| 2   | `ServoControlFSM.ino` | Action FSM        | Servo control, struct logic     | `ActionParams`, sweep         |
| 3   | `SensorLogFSM.ino`    | Logging FSM       | Input, state-based telemetry    | analogRead, logging           |
| 4   | `ConditionalFSM.ino`  | Event FSM         | Buttons, conditional logic      | `shouldAdvance()`             |
| 5   | `MissionDeckFSM.ino`  | Full Pipeline FSM | Input → Action → Feedback → Log | Servo, sensor, chained states |
| 6   | **Your Choice**       | Chain/Meta FSM    | Scripted mission, config-driven | Table-based, parameterized    |

---

## ✅ Current Lab 5: What's Inside Now

```cpp
StateConfig states[] = {
  { "WAIT_FOR_OBJECT", waitForObject, 1000, "MOVE_SERVO", nullptr, nullptr },
  { "MOVE_SERVO", moveServoReveal, 1000, "SHOW_LED", servoEnter90, servoExitDetach },
  { "SHOW_LED", showLEDFeedback, 1000, "LOG", nullptr, nullptr },
  { "LOG", logResult, 1000, "WAIT_FOR_OBJECT", nullptr, nullptr }
};
```

Currently:

* Each state is **hardcoded**
* `servoEnter90()` rotates to `90` via `myServo.write(90);`
* There’s no reusable `ActionParams` struct yet

---

## 🧱 What We Can Do (to Align with Lab 2 and Future Lab 6)

### 🔁 Replace this:

```cpp
myServo.write(90);
```

### ✅ With this:

```cpp
ActionParams moveAction = { SERVO_PIN, 90, 1000 };

void servoEnterWithParams(ActionParams action) {
  myServo.attach(action.pin);
  Serial.print("[SERVO] Rotating to ");
  Serial.println(action.angle);
  myServo.write(action.angle);
  delay(action.duration);  // or use FSM duration
  myServo.detach();
}
```

Or even better — plug this into an **`action_handler()`** function so Lab 6 can load actions from a config table.

---

## ✍️ Your Next Decision:

Do you want:

1. ✅ A **minimal refactor of Lab 5** to use a single `ActionParams` struct?
2. 🔁 A **multi-action table** (e.g., `ActionParams[]`) and handler?
3. 📦 A more generalized **Lab 6-style loader** where all FSM behaviors can be declarative?

Let’s choose 1️⃣ for now if the goal is to **teach via clarity** — then we can step up to full table-based chaining in Lab 6. Shall I upgrade `MissionDeckFSM.ino` now to use the minimal `ActionParams` setup?
