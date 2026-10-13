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

## 🔌 Circuit Diagram

Each ESP8266 device is powered by a 3.7V lithium battery through an AMS1117 3.3V buck-boost converter.

### Power Supply Connection (Per Device)

```text
   ┌─────────────────┐
   │  3.7V Lithium   │
   │    Battery      │
   │   (+ )   (-)    │
   └───┬─────────┬───┘
       │         │
   ┌───▼─────────▼───┐
   │   AMS1117       │
   │  Buck-Boost     │
   │  IN+     IN-    │
   │                 │
   │  OUT+    OUT-   │
   └───┬─────────┬───┘
       │         │
   ┌───▼─────────▼───┐
   │   ESP8266       │
   │   NodeMCU       │
   │   3V3     GND   │
   └─────────────────┘




Wiring Table
From	To	Voltage
Battery (+)	AMS1117 IN+	3.7V
Battery (-)	AMS1117 IN-	GND
AMS1117 OUT+	ESP8266 3V3	3.3V
AMS1117 OUT-	ESP8266 GND	GND
⚠️ Important: Set the AMS1117 output to exactly 3.3V using a multimeter before connecting it to the ESP8266. Never connect 5V directly to the 3V3 pin.

🚀 Setup Procedure
Step 1: Hardware Assembly
Connect the 3.7V lithium battery to the AMS1117 input (IN+ and IN-).

Connect the AMS1117 output (OUT+ and OUT-) to the ESP8266 (3V3 and GND).

Verify the AMS1117 output voltage is exactly 3.3V using a multimeter.

Repeat the same steps for the second device.

Step 2: Upload Code
Connect the ESP8266 to your laptop via USB.

Open Arduino IDE.

Select board: NodeMCU 1.0 (ESP-12E Module).

Select the correct COM port.

Set AP_SSID = "TriTro A" for Device 1, then click Upload.

Change to AP_SSID = "TriTro K" for Device 2, then click Upload.

Step 3: Power On
Disconnect the USB cable.

Power both devices from their lithium batteries.

Both devices will boot up and start their WiFi hotspots.

Step 4: Connect Your Phone
On your phone, open WiFi Settings.

Connect to the hotspot:

Phone 1 → TriTro A (Password: 12345678)

Phone 2 → TriTro K (Password: 12345678)

It will show "Connected, no internet" — this is normal.

Step 5: Open The Chat App
Open Chrome browser on your phone.

Type in the address bar: 192.168.4.1

The intro screen will appear.

Tap NEXT → START.

You're now in the chat!

Step 6: Start Chatting
Type a message in the input box.

Tap the Send button (➤).

The message will appear on the other device instantly.

text

MEOWWWWWWWWWWW....
