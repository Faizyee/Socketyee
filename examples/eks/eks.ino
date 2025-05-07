#include <ESP8266WiFi.h>
#include "IketsServer.h"

const char* ssid = "wifi";
const char* pass = "pass";

IketsServer ws(81);

IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  ws.begin();
  ws.onMessage([](String msg){
    Serial.println("Received: " + msg);
    ws.send("Echo: " + msg);
  });
}

void loop() {
  ws.loop();
}
