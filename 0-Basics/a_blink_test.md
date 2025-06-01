# a_blink_test.md – Blink Test Exercise

## 🔧 Objective
Make sure your ESP32-S3 board is working correctly by:
- Connecting it to your computer
- Uploading the `blink.ino` program
- Seeing it blink an LED
- Reading personalized messages in the Serial Monitor
- Pressing the "RESET" / "EN" button (silver and black left side) to restart the program

---

## 🛠️ Arduino IDE Settings (Lab Workstations)
Use these exact settings to match your lab desktop environment:

| Setting                     | Value                          |
|----------------------------|---------------------------------|
| **Board**                  | `ESP32S3 Dev Module`           |
| **Port**                   | `COMx` via **usbserial**       |
| **CPU Frequency**          | `240MHz (WiFi)`                |
| **Flash Size**             | `8MB (64Mb)`                   |
| **PSRAM**                  | `OPI PSRAM`                    |
| **Upload Mode**            | `UART0 / Hardware CDC`         |
| **Upload Speed**           | `921600`                       |
| **Partition Scheme**       | `8MB with spiffs`              |
| **Erase Flash Before Upload** | `Enabled`                  |
| **Serial Monitor Baud Rate** | `115200`                    |

[![Freenove ESP32S3 Config](https://github.com/RudyMartin/esp32/blob/main/0-Basics/Arduino_Configuration.png)](https://github.com/RudyMartin/esp32/blob/main/0-Basics/Arduino_Configuration.png)]

---

## 📂 File to Open
```
0-Basics/
└── blink.ino
```

---

## ✅ Steps to Complete

1. **Plug in your ESP32-S3 board**  
   Use the USB-C cable to connect it to your lab computer.

2. **Open `blink.ino` in Arduino IDE**

3. **Set your Username**  
   Find the line:
   ```cpp
   String username = "your_name_here";
   ```
   Change it to your name, like:
   ```cpp
   String username = "Jordan";
   ```

4. **Set Arduino Board & Port**  
   - Tools → Board → `ESP32S3 Dev Module`
   - Tools → Port → `COMx` (usbserial device)

5. **Upload the Code**  
   Click the ▶️ Upload button. Wait for "Done uploading."

6. **Open Serial Monitor**  
   Tools → Serial Monitor → Set baud to `115200`
   You should see:
   ```
   💡 Artemis Lab 1: Blink Test
   👋 Hello, Jordan!
   ✅ Your ESP32-S3 is working and the LED is blinking.
   ```

7. **Watch the LED Blink**
   It will turn on/off every second.

8. **Try the Reset Button**  
   Press the **EN** button on your board. The program will restart.

---

## 🧪 Troubleshooting
- Serial Monitor shows nothing? → Check your COM port + baud = `115200`
- Upload fails? Try holding BOOT while clicking upload, then release.
- No blinking? Check if LED is on a different GPIO (e.g., 10 or 2)

---

## ✅ Success Checklist
- [ ] Board connected
- [ ] Username edited
- [ ] Code uploaded
- [ ] LED is blinking
- [ ] Welcome message appears in Serial Monitor
- [ ] Reset button restarts the program

You're ready to move on to **servo control or FSM labs**! 🧠💡
