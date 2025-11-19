#include <WiFi.h>
#include <WebServer.h>

int motorPins[8] = {32, 26, 14, 27, 25, 33, 12, 13};

const char* ssid = "Rushi";
const char* password = "11111111";

WebServer server(80);

String statusMsg = "Idle";

bool runningPattern = false;
int currentPattern = 0;
int currentMotor = 0;
unsigned long lastUpdate = 0;
int stepTime = 2000;
int currentIntensity = 128;

void stopAllMotors() {
  for (int i = 0; i < 8; i++) {
    analogWrite(motorPins[i], 0);
  }
  runningPattern = false;
  statusMsg = "Stopped!";
}

void startPattern(int pattern) {
  stopAllMotors();
  currentPattern = pattern;
  currentMotor = (pattern == 1) ? 0 : 7;
  runningPattern = true;
  lastUpdate = millis();
  statusMsg = "Pattern " + String(pattern) + " Running (" 
              + String((currentIntensity * 100) / 255) + "%, Delay: " 
              + String(stepTime/1000.0, 2) + "s)";
}

void handlePattern() {
  if (!runningPattern) return;

  unsigned long now = millis();
  if (now - lastUpdate >= stepTime) {
    lastUpdate = now;

    if ((currentPattern == 1 && currentMotor < 8) ||
        (currentPattern == 2 && currentMotor >= 0)) {
      
      analogWrite(motorPins[currentMotor], currentIntensity);

      if (currentPattern == 1) currentMotor++;
      else currentMotor--;

    } else {
      for (int i = 0; i < 8; i++) {
        analogWrite(motorPins[i], currentIntensity);
      }
      delay(2000);
      stopAllMotors();
    }
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 8; i++) {
    pinMode(motorPins[i], OUTPUT);
    analogWrite(motorPins[i], 0);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>REVEN Motor Control</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          background: linear-gradient(to right, #667eea, #764ba2);
          color: white;
          text-align: center;
          padding: 50px;
        }
        h1 { font-size: 2.5em; margin-bottom: 20px; }
        button, input {
          background: #ff9800; border: none; color: white;
          padding: 12px 20px; font-size: 16px;
          margin: 10px; border-radius: 10px; cursor: pointer;
          transition: 0.3s;
        }
        button:hover { background: #e68900; }
        input {
          background: white; color: black; width: 100px;
          text-align: center; border: 2px solid #ff9800;
        }
        #status { margin-top: 30px; font-size: 20px; font-weight: bold; }
      </style>
      <script>
        async function sendCommand(cmd) {
          await fetch('/' + cmd);
          updateStatus();
        }
        async function setIntensity(value) {
          await fetch('/intensity?val=' + value);
          updateStatus();
        }
        async function setDelay(value) {
          await fetch('/delay?val=' + value);
          updateStatus();
        }
        async function setCustomDelay() {
          let val = document.getElementById("customDelay").value;
          if(val) {
            await fetch('/delay?val=' + val);
            updateStatus();
          }
        }
        async function updateStatus() {
          let res = await fetch('/status');
          let text = await res.text();
          document.getElementById("status").innerHTML = "Status: " + text;
        }
        setInterval(updateStatus, 1000);
      </script>
    </head>
    <body onload="updateStatus()">
      <h1>REVEN Motor Control</h1>

      <h2>Select Intensity</h2>
      <button onclick="setIntensity(64)">25%</button>
      <button onclick="setIntensity(128)">50%</button>
      <button onclick="setIntensity(255)">100%</button>

      <h2>Select Delay (Step Time)</h2>
      <button onclick="setDelay(500)">0.5s</button>
      <button onclick="setDelay(1000)">1s</button>
      <button onclick="setDelay(2000)">2s</button>
      <button onclick="setDelay(5000)">5s</button>
      <br><br>
      <input type="number" id="customDelay" placeholder="ms">
      <button onclick="setCustomDelay()">Set Custom</button>

      <h2>Choose Pattern</h2>
      <button onclick="sendCommand('pattern1')">Start Pattern 1</button>
      <button onclick="sendCommand('pattern2')">Start Pattern 2</button>
      <button onclick="sendCommand('stop')">STOP</button>

      <div id="status">Status: Idle</div>
    </body>
    </html>
    )rawliteral";
    server.send(200, "text/html", html);
  });

  server.on("/pattern1", []() {
    startPattern(1);
    server.send(200, "text/plain", "Pattern 1 Started");
  });

  server.on("/pattern2", []() {
    startPattern(2);
    server.send(200, "text/plain", "Pattern 2 Started");
  });

  server.on("/stop", []() {
    stopAllMotors();
    server.send(200, "text/plain", "Stopped!");
  });

  server.on("/intensity", []() {
    if (server.hasArg("val")) {
      currentIntensity = server.arg("val").toInt();
      statusMsg = "Intensity set to " + String((currentIntensity * 100) / 255) + "%";
    }
    server.send(200, "text/plain", statusMsg);
  });

  server.on("/delay", []() {
    if (server.hasArg("val")) {
      stepTime = server.arg("val").toInt();
      statusMsg = "Step delay set to " + String(stepTime / 1000.0, 2) + "s";
    }
    server.send(200, "text/plain", statusMsg);
  });

  server.on("/status", []() {
    server.send(200, "text/plain", statusMsg);
  });

  server.begin();
}

void loop() {
  server.handleClient();
  handlePattern();
}
