/*
   ESP8266 Offline Chat — Dr. T. Thimmaiah Institute of Technology, KGF
   Department of Electronics and Communication Engineering
   
   Project: Offline Communication System using ESP8266 + ESP-NOW
   Team: names and roll numbers
   
   Upload the SAME code to both ESP8266 devices.
   For Device 1: AP_SSID = "TriTro A"
   For Device 2: AP_SSID = "TriTro K"
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

// ==================== FORWARD DECLARATIONS ====================
void saveToFlash();
void loadFromFlash();
void onDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len);
void handleRoot();
void handleSend();
void handleGet();
void handleManifest();
void handleServiceWorker();
void handleMessageCount();

// ==================== CONFIGURATION ====================
const char* AP_SSID = "TriTro A";        // <-- CHANGE TO "TriTro K" FOR DEVICE 2
const char* AP_PASS = "12345678";

// ==================== GLOBALS ====================
ESP8266WebServer server(80);
uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
String messageLog = "";
int messageCount = 0;
unsigned long lastSaveTime = 0;

// ==================== HTML PAGE ====================
const char* HTML_PAGE = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no, viewport-fit=cover">
<meta name="theme-color" content="#1f2c33">
<meta name="mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<link rel="manifest" href="/manifest.json">
<link rel="icon" href="/icon.svg">
<title>ECE Offline Chat</title>
<style>
  html, body { height: 100%; width: 100%; overflow: hidden; margin: 0; padding: 0; }
  * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
  body { font-family: -apple-system, 'Segoe UI', Roboto, Arial, sans-serif; background: #0b141a; color: white; }
  
  .screen {
    position: fixed;
    top: 0; left: 0; right: 0; bottom: 0;
    width: 100vw;
    height: 100vh;
    height: 100dvh;
    display: none;
    flex-direction: column;
    opacity: 0;
    transition: opacity 0.4s ease;
    overflow: hidden;
  }
  .screen.active { display: flex; opacity: 1; }
  
  /* ============ INTRO SCREEN ============ */
  #introScreen { align-items: center; justify-content: center; padding: 20px; text-align: center; background: #4A7BC8; }
  #introScreen::before {
    content: ""; position: absolute; top: 0; left: 0; right: 0; bottom: 0;
    background-image: url("data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='400' height='400' viewBox='0 0 400 400'><g fill='none' stroke='white' stroke-width='1.5' opacity='0.15'><circle cx='60' cy='60' r='25'/><circle cx='60' cy='60' r='18'/><circle cx='60' cy='60' r='8' fill='white'/><line x1='40' y1='60' x2='80' y2='60'/><line x1='60' y1='40' x2='60' y2='80'/><rect x='310' y='40' width='50' height='35' rx='3'/><circle cx='335' cy='100' r='12'/><rect x='30' y='310' width='60' height='40' rx='3'/><circle cx='60' cy='330' r='5' fill='white'/><path d='M 290 320 L 310 300 L 350 300 L 370 320 Z'/><circle cx='330' cy='355' r='15'/><path d='M 100 180 L 120 160 L 140 180 L 140 220 L 100 220 Z'/><circle cx='120' cy='200' r='10'/><circle cx='380' cy='180' r='15'/><circle cx='380' cy='180' r='8' fill='white'/><text x='380' y='185' font-size='10' fill='white' text-anchor='middle' opacity='0.5'>IC</text></g></svg>");
    background-size: 500px 500px; background-position: center; z-index: 0; pointer-events: none;
  }
  #introScreen::after {
    content: ""; position: absolute; top: 0; left: 0; right: 0; bottom: 0;
    background: radial-gradient(circle at center, rgba(74,123,200,0.3) 0%, rgba(30,58,95,0.85) 100%);
    z-index: 1; pointer-events: none;
  }
  #introScreen > * { position: relative; z-index: 2; }
  
  .college-name { font-size: 22px; font-weight: 900; margin-bottom: 10px; letter-spacing: 1px; color: #FFD700; animation: slideUp 0.8s ease-out 0.3s both; text-shadow: 0 3px 10px rgba(0,0,0,0.8); line-height: 1.3; padding: 0 10px; }
  .dept-name { font-size: 16px; color: #ffffff; margin-bottom: 40px; animation: slideUp 0.8s ease-out 0.6s both; text-shadow: 0 3px 10px rgba(0,0,0,0.8); font-weight: 500; line-height: 1.4; padding: 0 10px; }
  @keyframes slideUp { from { opacity: 0; transform: translateY(20px); } to { opacity: 1; transform: translateY(0); } }
  
  .app-title { font-size: 38px; font-weight: 900; color: white; margin-bottom: 14px; letter-spacing: 3px; text-shadow: 0 4px 15px rgba(0,0,0,0.9); animation: slideUp 0.8s ease-out 0.9s both; }
  .app-subtitle { font-size: 15px; color: #e0e0e0; margin-bottom: 50px; text-shadow: 0 3px 10px rgba(0,0,0,0.8); animation: slideUp 0.8s ease-out 1.1s both; font-weight: 500; }
  
  .connection-status { font-size: 18px; color: #00ff88; font-weight: bold; margin-bottom: 20px; animation: pulse 1.5s infinite; text-shadow: 0 3px 10px rgba(0,0,0,0.8); letter-spacing: 2px; }
  @keyframes pulse { 0%, 100% { opacity: 0.6; } 50% { opacity: 1; } }
  
  .loading-bar { width: 280px; max-width: 85%; height: 6px; background: rgba(255,255,255,0.2); border-radius: 6px; overflow: hidden; margin-bottom: 12px; }
  .loading-fill { height: 100%; width: 0%; background: linear-gradient(90deg, #00ff88, #FFD700); border-radius: 6px; animation: loadProgress 5s linear forwards; }
  @keyframes loadProgress { from { width: 0%; } to { width: 100%; } }
  .loading-percent { font-size: 15px; color: #e0e0e0; margin-bottom: 50px; text-shadow: 0 3px 10px rgba(0,0,0,0.8); font-weight: 500; }
  
  .btn { padding: 18px 70px; border: none; border-radius: 35px; background: #FFD700; color: #2E5B9E; font-size: 20px; font-weight: 900; letter-spacing: 3px; cursor: pointer; box-shadow: 0 8px 25px rgba(255,215,0,0.5); transition: all 0.3s; animation: fadeIn 0.5s ease-out 5s both; }
  .btn:active { transform: scale(0.95); }
  @keyframes fadeIn { from { opacity: 0; } to { opacity: 1; } }
  
  /* ============ TEAM SCREEN ============ */
  #teamScreen { align-items: center; justify-content: center; padding: 20px; text-align: center; background: linear-gradient(135deg, #1e3a5f 0%, #2E5B9E 100%); overflow-y: auto; }
  .team-title { font-size: 26px; font-weight: 900; color: #FFD700; margin-bottom: 10px; letter-spacing: 3px; }
  .team-subtitle { font-size: 15px; color: #b0c4de; margin-bottom: 30px; font-weight: 500; }
  .member { background: rgba(255,255,255,0.1); border: 1px solid rgba(255,215,0,0.3); border-radius: 14px; padding: 16px 20px; margin: 10px 0; width: 320px; max-width: 92%; display: flex; align-items: center; gap: 14px; text-align: left; animation: slideUp 0.6s ease-out both; }
  .member:nth-child(3) { animation-delay: 0.2s; }
  .member:nth-child(4) { animation-delay: 0.4s; }
  .member:nth-child(5) { animation-delay: 0.6s; }
  .member-avatar { width: 52px; height: 52px; border-radius: 50%; background: #FFD700; color: #2E5B9E; display: flex; align-items: center; justify-content: center; font-weight: 900; font-size: 22px; flex-shrink: 0; }
  .member-info { flex: 1; }
  .member-name { font-size: 18px; font-weight: bold; color: white; }
  .member-roll { font-size: 14px; color: #b0c4de; margin-top: 3px; }
  
  /* ============ CHAT SCREEN ============ */
  #chatScreen { background: #0b141a; }
  .header { background: #1f2c33; color: white; padding: 10px 12px; display: flex; align-items: center; border-bottom: 1px solid #2a3942; flex-shrink: 0; }
  .header-avatar { width: 44px; height: 44px; border-radius: 50%; background: #2E5B9E; display: flex; align-items: center; justify-content: center; font-weight: bold; margin-right: 12px; font-size: 18px; color: white; }
  .header-text { font-size: 17px; font-weight: 600; }
  .header-sub { font-size: 12px; color: #8696a0; margin-top: 2px; }
  .header-sub.online { color: #00a884; }
  
  #chat { flex: 1; overflow-y: auto; padding: 12px 8px; background: #0b141a; display: flex; flex-direction: column; }
  .msg { padding: 8px 12px; margin: 3px 0; border-radius: 8px; max-width: 78%; width: fit-content; word-wrap: break-word; font-size: 15px; line-height: 1.4; display: inline-block; }
  .me { background: #005c4b; margin-left: auto; border-top-right-radius: 2px; }
  .them { background: #202c33; margin-right: auto; border-top-left-radius: 2px; }
  .time { font-size: 10px; color: #8696a0; text-align: right; margin-top: 3px; display: flex; justify-content: flex-end; align-items: center; gap: 4px; }
  .me .time { color: #a0d8c8; }
  
  .input-area { display: flex; padding: 8px; background: #1f2c33; align-items: center; flex-shrink: 0; gap: 6px; }
  input { flex: 1; padding: 12px 16px; border: none; border-radius: 24px; font-size: 16px; background: #2a3942; color: white; outline: none; }
  input::placeholder { color: #8696a0; }
  .send-btn { width: 46px; height: 46px; border-radius: 50%; background: #00a884; color: white; border: none; font-size: 20px; display: flex; align-items: center; justify-content: center; cursor: pointer; flex-shrink: 0; }
  .send-btn:active { transform: scale(0.95); }
</style>
</head>
<body>

<!-- ============ INTRO SCREEN ============ -->
<div class="screen active" id="introScreen">
  <div class="college-name">DR. T. THIMMAIAH<br>INSTITUTE OF TECHNOLOGY</div>
  <div class="dept-name">Department of Electronics and<br>Communication Engineering</div>
  <div class="app-title">📡 ECE OFFLINE CHAT</div>
  <div class="app-subtitle">Offline Communication System using ESP8266</div>
  <div class="connection-status">● TriTro Network Ready</div>
  <div class="loading-bar"><div class="loading-fill"></div></div>
  <div class="loading-percent">Loading 100%</div>
  <button class="btn" onclick="showTeam()">NEXT</button>
</div>

<!-- ============ TEAM SCREEN ============ -->
<div class="screen" id="teamScreen">
  <div class="team-title">PROJECT TEAM</div>
  <div class="team-subtitle">Department of ECE</div>
  <div class="member">
    <div class="member-avatar">Y</div>
    <div class="member-info">
      <div class="member-name">Yogesh SV</div>
      <div class="member-roll">1GV23EC061</div>
    </div>
  </div>
  <div class="member">
    <div class="member-avatar">P</div>
    <div class="member-info">
      <div class="member-name">Punith A</div>
      <div class="member-roll">1GV23EC040</div>
    </div>
  </div>
  <div class="member">
    <div class="member-avatar">D</div>
    <div class="member-info">
      <div class="member-name">Dharshan B</div>
      <div class="member-roll">1GV23EC015</div>
    </div>
  </div>
  <button class="btn" style="margin-top: 30px; animation: none; opacity: 1;" onclick="showChat()">START</button>
</div>

<!-- ============ CHAT SCREEN ============ -->
<div class="screen" id="chatScreen">
  <div class="header">
    <div class="header-avatar">📡</div>
    <div>
      <div class="header-text">ECE Offline Chat</div>
      <div class="header-sub online">● Online — TriTro Network</div>
    </div>
  </div>
  <div id="chat"></div>
  <div class="input-area">
    <input type="text" id="msgInput" placeholder="Message" autocomplete="off" onkeypress="if(event.key==='Enter')sendMsg()">
    <button class="send-btn" onclick="sendMsg()">➤</button>
  </div>
</div>

<script>
  var lastCount = -1;
  
  function showScreen(id) {
    document.querySelectorAll('.screen').forEach(s => s.classList.remove('active'));
    document.getElementById(id).classList.add('active');
  }
  function showTeam() { showScreen('teamScreen'); }
  function showChat() { 
    showScreen('chatScreen');
    loadMessages(true);
    setInterval(checkForNewMessages, 1500);
  }
  
  function checkForNewMessages() {
    fetch('/count').then(r => r.text()).then(count => {
      var newCount = parseInt(count);
      if (newCount !== lastCount) {
        lastCount = newCount;
        loadMessages(false);
      }
    }).catch(e => {});
  }
  
  function loadMessages(scrollAlways) {
    fetch('/get').then(r => r.text()).then(data => {
      var chat = document.getElementById('chat');
      chat.innerHTML = data;
      if (scrollAlways) {
        chat.scrollTop = chat.scrollHeight;
      } else {
        chat.scrollTop = chat.scrollHeight;
      }
    }).catch(e => {});
  }
  
  function sendMsg() {
    var input = document.getElementById('msgInput');
    var msg = input.value.trim();
    if (msg === '') return;
    input.value = '';
    fetch('/send?msg=' + encodeURIComponent(msg))
      .then(() => setTimeout(() => loadMessages(true), 200));
  }
</script>
</body>
</html>
)rawliteral";

// ==================== ICON SVG ====================
const char* ICON_SVG = R"rawliteral(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 192 192"><rect width="192" height="192" rx="40" fill="#1f2c33"/><circle cx="96" cy="96" r="70" fill="#00a884"/><text x="96" y="115" font-size="70" text-anchor="middle" fill="white" font-weight="bold" font-family="Arial">ECE</text></svg>)rawliteral";

// ==================== ESP-NOW RECEIVE ====================
void onDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  char msg[200];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';

  unsigned long secs = millis() / 1000;
  int h = (secs / 3600) % 24;
  int m = (secs / 60) % 60;
  char timeStr[8];
  sprintf(timeStr, "%02d:%02d", h, m);

  String html = "<div class='msg them'>";
  html += String(msg);
  html += "<div class='time'>";
  html += String(timeStr);
  html += "</div></div>";
  messageLog += html;
  messageCount++;

  saveToFlash();
  Serial.print("Received: ");
  Serial.println(msg);
}

// ==================== WEB HANDLERS ====================
void handleRoot() { server.send(200, "text/html", HTML_PAGE); }
void handleIcon() { server.send(200, "image/svg+xml", ICON_SVG); }
void handleMessageCount() { server.send(200, "text/plain", String(messageCount)); }

void handleSend() {
  if (server.hasArg("msg")) {
    String msg = server.arg("msg");
    msg.trim();
    if (msg.length() > 0 && msg.length() < 200) {
      unsigned long secs = millis() / 1000;
      int h = (secs / 3600) % 24;
      int m = (secs / 60) % 60;
      char timeStr[8];
      sprintf(timeStr, "%02d:%02d", h, m);

      String html = "<div class='msg me'>";
      html += msg;
      html += "<div class='time'>";
      html += String(timeStr);
      html += " ✓✓</div></div>";
      messageLog += html;
      messageCount++;

      uint8_t data[200];
      memcpy(data, msg.c_str(), msg.length());
      esp_now_send(broadcastMac, data, msg.length());
      saveToFlash();
      
      Serial.print("Sent: ");
      Serial.println(msg);
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleGet() { server.send(200, "text/html", messageLog); }

void handleManifest() {
  String manifest = R"({
    "name": "ECE Offline Chat",
    "short_name": "ECE Chat",
    "start_url": "/",
    "display": "standalone",
    "orientation": "portrait",
    "background_color": "#1f2c33",
    "theme_color": "#1f2c33",
    "icons": [{
      "src": "/icon.svg",
      "sizes": "192x192",
      "type": "image/svg+xml",
      "purpose": "any maskable"
    }]
  })";
  server.send(200, "application/json", manifest);
}

void handleServiceWorker() {
  String sw = R"(
    self.addEventListener('install', e => self.skipWaiting());
    self.addEventListener('activate', e => self.clients.claim());
    self.addEventListener('fetch', e => {
      e.respondWith(fetch(e.request).catch(() => new Response('')));
    });
  )";
  server.send(200, "application/javascript", sw);
}

// ==================== FLASH PERSISTENCE ====================
void saveToFlash() {
  if (millis() - lastSaveTime < 5000) return;
  lastSaveTime = millis();
  File f = LittleFS.open("/chatlog.txt", "w");
  if (f) { f.print(messageLog); f.close(); }
}

void loadFromFlash() {
  File f = LittleFS.open("/chatlog.txt", "r");
  if (f) { 
    messageLog = f.readString(); 
    int count = 0;
    int idx = 0;
    while ((idx = messageLog.indexOf("class='msg", idx)) != -1) {
      count++;
      idx++;
    }
    messageCount = count;
    f.close();
  }
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n=== ECE OFFLINE CHAT ===");
  Serial.print("Device SSID: ");
  Serial.println(AP_SSID);

  LittleFS.begin();
  loadFromFlash();

  WiFi.mode(WIFI_AP_STA);
  
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed!");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(onDataRecv);
  esp_now_add_peer(broadcastMac, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("Hotspot: ");
  Serial.println(AP_SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/icon.svg", handleIcon);
  server.on("/send", handleSend);
  server.on("/get", handleGet);
  server.on("/count", handleMessageCount);
  server.on("/manifest.json", handleManifest);
  server.on("/sw.js", handleServiceWorker);
  server.begin();

  Serial.println("Ready! Open http://192.168.4.1\n");
}

void loop() {
  server.handleClient();
}
