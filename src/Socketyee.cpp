// Socketyee.cpp
#include "Socketyee.h"
#if defined(ESP8266)
  #include <Hash.h>
#elif defined(ESP32)
  #include "mbedtls/sha1.h"
#endif
#include <base64.h>

Socketyee::Socketyee() {}

void Socketyee::beginServer(uint16_t port) {
  mode = SERVER_MODE;
  server = WiFiServer(port);
  server.begin();
}

void Socketyee::beginClient(const String& h, uint16_t p, const String& pa) {
  host = h;
  port = p;
  path = pa;
  mode = CLIENT_MODE;
  isClientWebSocket = false;
}

void Socketyee::onMessage(std::function<void(String)> callback) {
  onMessageCallback = callback;
}

void Socketyee::loop() {
  if (mode == SERVER_MODE) {
    if (!client || !client.connected()) {
      client = server.available();
      isWebSocket = false;
    }
    if (client && client.connected()) {
      handleClient();
    }
  } else if (mode == CLIENT_MODE) {
    if (!wsClient.connected()) {
      if (wsClient.connect(host.c_str(), port)) {
        String clientKey = "dGhlIHNhbXBsZSBub25jZQ==";
        wsClient.print("GET " + path + " HTTP/1.1\r\n");
        wsClient.print("Host: " + host + "\r\n");
        wsClient.print("Upgrade: websocket\r\n");
        wsClient.print("Connection: Upgrade\r\n");
        wsClient.print("Sec-WebSocket-Key: " + clientKey + "\r\n");
        wsClient.print("Sec-WebSocket-Version: 13\r\n\r\n");
      }
    } else if (!isClientWebSocket && wsClient.available()) {
      String line = wsClient.readStringUntil('\n');
      if (line.indexOf("101") >= 0) {
        isClientWebSocket = true;
      }
    } else if (isClientWebSocket && wsClient.available()) {
      handleClientFrame();
    }
  }
}

void Socketyee::handleClient() {
  if (!isWebSocket && client.available()) {
    String req = client.readStringUntil('\r');
    if (req.startsWith("GET")) {
      while (client.available()) {
        String line = client.readStringUntil('\r');
        if (line.indexOf("Sec-WebSocket-Key: ") >= 0) {
          String clientKey = line.substring(19);
          clientKey.trim();
          String acceptKey = generateAcceptKey(clientKey);
          client.println("HTTP/1.1 101 Switching Protocols");
          client.println("Upgrade: websocket");
          client.println("Connection: Upgrade");
          client.print("Sec-WebSocket-Accept: ");
          client.println(acceptKey);
          client.println();
          isWebSocket = true;
        }
      }
    }
  } else if (isWebSocket && client.available()) {
    handleServerFrame();
  }
}

void Socketyee::handleServerFrame() {
  uint8_t opcode = client.read();
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

void Socketyee::handleClientFrame() {
  uint8_t opcode = wsClient.read();
  uint8_t len = wsClient.read() & 0x7F;

  char data[len + 1];
  for (uint8_t i = 0; i < len; i++) {
    data[i] = wsClient.read();
  }
  data[len] = '\0';

  if (onMessageCallback) {
    onMessageCallback(String(data));
  }
}

void Socketyee::send(String message) {
  if (mode == SERVER_MODE && client && client.connected() && isWebSocket) {
    client.write(0x81);
    uint8_t len = message.length();
    client.write(len);
    client.write((const uint8_t*)message.c_str(), len);
  } else if (mode == CLIENT_MODE && wsClient && wsClient.connected() && isClientWebSocket) {
    wsClient.write(0x81);
    uint8_t len = message.length();
    wsClient.write(len | 0x80);
    uint8_t mask[4] = {random(0,255), random(0,255), random(0,255), random(0,255)};
    wsClient.write(mask, 4);
    for (int i = 0; i < len; i++) {
      wsClient.write(message[i] ^ mask[i % 4]);
    }
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
