
# WiFi-Controlled Multi-Channel Vibration Motor Driver System  
### Custom Hardware + ESP32 Firmware (Designed and Developed by Me)

This project is a complete multi-motor vibration control system built using a **custom-designed vibration motor driver board** and an **ESP32** microcontroller.  
The system allows real-time control of **8 vibration motors** through a WiFi-hosted web interface.

---

##  Features

###  Hardware (Designed by Me)
- Custom PCB for **8-channel vibration motor driving**
- Independent PWM control per motor
- High-current driver stage suitable for haptic/massage motors
- ESP32 integrated with driver circuitry
- Stable power distribution & motor protection design

###  Firmware (ESP32)
- Built-in WebServer (no external server required)
- Non-blocking motor pattern engine (`millis()`)
- Two vibration patterns:
  - **Pattern 1:** Motor 1 → Motor 8 sequential activation  
  - **Pattern 2:** Motor 8 → Motor 1 sequential activation
- PWM intensity control (25%, 50%, 100%, custom)
- Adjustable step delay (0.5s – 5s + custom)
- Final effect: all motors ON for 2 seconds at end of each pattern
- Safe STOP command to shut down all motors
- Real-time status updates to webpage

---

##  Web Dashboard
Accessible from any mobile or PC browser.

### Includes:
- Intensity Selection  
- Delay/Timing Control  
- Pattern 1 / Pattern 2 Start  
- Custom Delay Input  
- STOP Button  
- Live Status Display  

Built with **HTML + CSS + JavaScript** hosted on the ESP32.

---

##  How to Use
1. Flash the ESP32 with the provided firmware.
2. Connect vibration motors to your custom hardware board.
3. Power the ESP32 + motor driver module.
4. ESP32 connects to WiFi using the configured SSID/password.
5. Open the ESP32 IP address in a browser.
6. Use the dashboard to control motors.

---

![vibmotorphoto 2025-11-18 at 6 19 06 PM](https://github.com/user-attachments/assets/3805d61c-3b10-4628-8faf-43dd5b66e55e)
![vibrnmotorprojphoto 2025-11-18 at 6 19 05 PM](https://github.com/user-attachments/assets/1e2b8996-3f4b-437b-807c-23f09cd2e235)

## Code Example
```cpp
server.on("/pattern1", []() {
  startPattern(1);
  server.send(200, "text/plain", "Pattern 1 Started");
});
'''

