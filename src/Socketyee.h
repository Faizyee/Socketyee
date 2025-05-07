#ifndef SOCKETYEE_H
#define SOCKETYEE_H

#include <WiFiClient.h>
#include <WiFiServer.h>
#include <functional>

class Socketyee {
  public:
    Socketyee(uint16_t port = 81);
    void begin();
    void loop();
    void onMessage(std::function<void(String)> callback);
    void send(String message);

  private:
    WiFiServer server;
    WiFiClient client;
    std::function<void(String)> onMessageCallback;
    bool isWebSocket = false;
    
    void handleClient();
    String generateAcceptKey(const String& clientKey);
    String base64Encode(const uint8_t *data, size_t len);
};

#endif
