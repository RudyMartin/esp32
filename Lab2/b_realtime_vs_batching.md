

### ⚙️ Real-Time vs Batch Inference – Options for Lab 2+

| **Approach**                   | **Description**                                                       | **Pros**                                             | **Cons**                                              | **When to Use**                    |
| ------------------------------ | --------------------------------------------------------------------- | ---------------------------------------------------- | ----------------------------------------------------- | ---------------------------------- |
| **Frame-by-frame Inference**   | Run inference immediately on each camera frame (FSM cycle).           | Simple FSM logic, easy to teach, real-time feedback  | Higher latency per frame, less efficient              | Lab 2–3, LED display output        |
| **Buffered Micro-Batch**       | Collect 2–5 frames before running inference as a mini-batch.          | Smoothes out noise, better accuracy from aggregation | Slightly more RAM needed, added delay                 | Capstone voting or majority logic  |
| **Rolling Average Prediction** | Maintain a moving window of last N predictions for stability.         | Filters out jittery predictions                      | Delayed response, adds state tracking complexity      | Capstone logging or alerts         |
| **Offloaded Batch**            | Stream images to an edge device or server for batched processing.     | Better model size/flexibility, shared compute        | Requires Wi-Fi stability and async logic              | Bonus or instructor-only extension |
| **Hybrid FSM Batching**        | FSM has a `BUFFERING` state before `CLASSIFY` that waits for N frames | Teachable structure, adjustable to kit RAM limits    | More complex logic, careful camera memory mgmt needed | Lab 5+ with servo reaction stage   |

---

### 🧪 What to Try in Lab 2.5+

Here’s how you might evolve Lab 2 incrementally toward batching logic:

#### Option A: FSM with “Classify Every 3 Frames”

```cpp
int frameCounter = 0;
void captureState() {
  captureFrame();  // camera_fb_t* fb = esp_camera_fb_get();
  frameCounter++;
  if (frameCounter >= 3) {
    currentState = CLASSIFY;
    frameCounter = 0;
  } else {
    currentState = WAIT;
  }
}
```

#### Option B: Simple Voting Buffer

```cpp
String buffer[3];
buffer[i++] = lastPrediction;
if (i == 3) {
  String final = majorityVote(buffer);
  // then transmit...
}
```

---

### 💡 In Your Capstone Kit

You might:

* Log every 5 predictions to the shared NAS only if 3/5 match
* Trigger servo action only if 2 sequential frames agree
* Use real-time streaming + batch fallback: transmit immediately, but confirm with a second opinion in background

---

Would you like me to:

* Add this as an **"Advanced Options" section** to the bottom of your current `lab2_capture_exercises.md`?
* Or insert it into a **separate teaching handout** called `lab2_batching_notes.md`?
