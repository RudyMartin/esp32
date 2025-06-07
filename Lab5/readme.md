
### ✅ **Lab 5 Focus Recap**

#### 👇 What Changes:

* **Lab 4**: Decisions combined model confidence *and* external context (e.g., light, temperature).
* **Lab 5**: Introduces **multi-step FSM workflows** (e.g., gate + servo + log + alert), based on both conditions and mission rules.

This is the **first full system** test. Students now build a **complete decision loop**, combining sensing, planning, action, and communication.

---

### 🧠 **Learning Goals:**

* Chain together **multiple FSM states**
* Route actions across different actuators (e.g., gates, servos, lights)
* Introduce **logging**, error fallback, and alerts
* Understand **mission protocol logic** (e.g., rules for when objects are allowed to pass)

---

### 🚦 Example:

A detected object might:

1. Trigger a gate to open if safe
2. Send a log to the mission dashboard
3. Blink an alert if conditions are violated

---

### ✅ **Script Name**

`lab5_mission_routing_agent.ino`
