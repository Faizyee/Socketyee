#include <Socketyee.h>

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

Socketyee socket;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  socket.beginServer(81);
  socket.onMessage([](String msg) {
    Serial.println("Received: " + msg);
    socket.send("Halo dari server!");
  });
}

void loop() {
  socket.loop();
}
