# 📡 ESP8266 Offline Chat

Offline chat system using two ESP8266 boards. No internet or SIM needed — just connect your phone to the ESP's WiFi and chat. Battery powered.

Two devices are used: **TriTro A** (Device 1) and **TriTro K** (Device 2). Both work vice versa. The given code is the same for both Device A and Device K — you can change the device names according to your needs in the code.

### ✏️ Code Changes You Can Make
*   Change your SSID and password according to your mobile hotspot.
*   Change the intro interface (branch, institute name, company names, etc.).
*   Change the team names and register numbers.

---

### 📖 Project Overview
An offline communication system built using two ESP8266 boards that allows users to chat within **100 to 300 meters outdoors**, without internet, cellular data, or a router. It creates a local Wi-Fi hotspot and uses **ESP-NOW** for wireless messaging.

### 🎓 Institution
**Dr. T. Thimmaiah Institute of Technology, KGF**
*Department of Electronics and Communication Engineering (ECE)*

### 👨‍💻 Project Team
*   **[Your Name]** – [Roll Number]
*   **[Your Name]** – [Roll Number]
*   **[Your Name]** – [Roll Number]
*   **[Your Name]** – [Roll Number]

### 🛠️ How It Works
1.  Each ESP8266 creates its own Wi-Fi hotspot (`TriTro A` and `TriTro K`).
2.  Users connect their phone to the hotspot and open `192.168.4.1` in a browser.
3.  Messages are sent between devices using the **ESP-NOW** protocol.
4.  The web interface uses a WhatsApp-style UI with a college-branded intro screen.

### 💻 Hardware Used
*   2x ESP8266 (NodeMCU)
*   2x Lithium 3.7V Batteries (connected to AMS1117 input)
*   2x AMS1117 3.3V Buck-Boost Converter (output directly connected to ESP 3V input and GND)
    *   **OR**
*   USB Power Cables / Power Banks

### 📸 Screenshots

**Intro Screen**
![Intro Screen](https://github.com/user-attachments/assets/3df2e30a-74e9-4f2e-a5af-f05e5266dcbd)

**Connected**
![Connected](https://github.com/user-attachments/assets/2af447bf-26c4-4629-81ce-752fa6a93382)

**Chat Screen**
![Chat Screen](https://github.com/user-attachments/assets/40ed0c95-e68b-4fa4-80ff-8ae2f2ba70f8)
