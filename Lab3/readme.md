## 🔁 Lab 3 Focus Recap

### 👇 What Happened:

**Lab 3** introduced the concept of an **autonomous agent that responds to predictions** using a finite state machine (FSM) and follows the MCP loop:

| Step       | Role in Lab 3                                          |
| ---------- | ------------------------------------------------------ |
| **Sense**  | Receive JSON result from LLM or classifier             |
| **Plan**   | Evaluate confidence value only (e.g., > 0.75 = accept) |
| **Act**    | Light up GPIO, LED, or buzzer to signal decision       |
| **Log**    | Send result to Serial (if available)                   |
| **Repeat** | Restart loop and process next input                    |

### 🎯 Key Agent Behavior:

* **Single-source decision-making**
  → Based **only on the confidence** value of the model output

### 🔧 What Students Built:

* Modular output functions (`lightGPIO`, `lightLED`, `buzzAlert`)
* Configurable display options (`AlertPoints`)
* A resilient FSM that could still run even if Serial or Matrix failed

---

### 🧠 Learning Goals of Lab 3:

* Use FSM to structure agent behavior
* Interpret model output in real time
* Translate model confidence into **physical action**
* Build reliable feedback using GPIO + LED + sound

---

### ✅ Script Name:

```plaintext
lab3_confidence_alert_agent.ino
```



