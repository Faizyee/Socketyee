// Socketyee.h
#ifndef SOCKETYEE_H
#define SOCKETYEE_H

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <functional>

class Socketyee {
  public:
    Socketyee();

    void beginServer(uint16_t port);

    void beginClient(const String& host, uint16_t port, const String& path = "/");

    void onMessage(std::function<void(String)> callback);

    void loop();

    void send(String message);

  private:
    enum Mode {
      SERVER_MODE,
      CLIENT_MODE
    } mode = SERVER_MODE;

    WiFiServer server = WiFiServer(81);
    WiFiClient client;

    WiFiClient wsClient;

    bool isWebSocket = false;
    bool isClientWebSocket = false;

    std::function<void(String)> onMessageCallback;

    String host;
    uint16_t port;
    String path;

    void handleClient();
    void handleClientFrame();
    void handleServerFrame();

    String generateAcceptKey(const String& clientKey);
};

#endif
