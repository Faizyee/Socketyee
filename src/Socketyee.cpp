#include "Socketyee.h"
#include <Arduino.h>
#if defined(ESP8266)
  #include <Hash.h>
#elif defined(ESP32)
  #include "mbedtls/sha1.h"
#endif
#include <base64.h>

Socketyee::Socketyee(uint16_t port) : server(port) {}

void Socketyee::begin() {
  server.begin();
}

void Socketyee::onMessage(std::function<void(String)> callback) {
  onMessageCallback = callback;
}

void Socketyee::loop() {
  if (!client || !client.connected()) {
    client = server.available();
    isWebSocket = false;
  }

  if (client && client.connected()) {
    handleClient();
  }
}

void Socketyee::handleClient() {
  if (!isWebSocket && client.available()) {
    String req = client.readStringUntil('\r');
    // Serial.println(req);
    if (req.startsWith("GET")) {
      while (client.available()) {
        String line = client.readStringUntil('\r');
        if (line.indexOf("Sec-WebSocket-Key: ") >= 0) {
          String clientKey = line.substring(19);
          clientKey.trim();
          // Serial.println(clientKey);
          String acceptKey = generateAcceptKey(clientKey);
          client.println("HTTP/1.1 101 Switching Protocols");
          client.println("Upgrade: websocket");
          client.println("Connection: Upgrade");
          client.print("Sec-WebSocket-Accept: ");
          client.println(acceptKey);
          client.println();
          // Serial.println(acceptKey);
          isWebSocket = true;
        }
      }
    }
  } else if (isWebSocket && client.available()) {
    uint8_t opcode = client.read();  // 0x81 = text
    // Serial.println(opcode);
    uint8_t len = client.read() & 0x7F;
    uint8_t mask[4];
    client.read(mask, 4);

    char data[len + 1];
    for (uint8_t i = 0; i < len; i++) {
      data[i] = client.read() ^ mask[i % 4];
    }
    data[len] = '\0';

    if (onMessageCallback) {
      onMessageCallback(String(data));
    }
  }
}

void Socketyee::send(String message) {
  if (client && client.connected() && isWebSocket) {
    client.write(0x81); // text frame
    uint8_t len = message.length();
    client.write(len);
    client.write((const uint8_t*)message.c_str(), len);
  }
}

String Socketyee::generateAcceptKey(const String& clientKey) {
  String key = clientKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  uint8_t sha1Result[20];

#if defined(ESP8266)
  sha1(key.c_str(), key.length(), sha1Result);

#elif defined(ESP32)
  mbedtls_sha1((const unsigned char*)key.c_str(), key.length(), sha1Result);

#endif

  return base64::encode(sha1Result, 20);
}
