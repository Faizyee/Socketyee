#include <Socketyee.h>

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

Socketyee socket;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  socket.beginClient("192.168.1.100", 81); // IP dan port server WebSocket
  socket.onMessage([](String msg) {
    Serial.println("Received from server: " + msg);
  });
}

void loop() {
  socket.loop();
  static unsigned long lastSent = 0;
  if (millis() - lastSent > 3000) {
    socket.send("Hello dari klien!");
    lastSent = millis();
  }
}
