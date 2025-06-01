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

