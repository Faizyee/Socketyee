# Socketyee - WebSocket Library for ESP8266 and ESP32

Socketyee is a lightweight and efficient WebSocket library designed for **ESP8266** and **ESP32** boards. It allows you to easily implement WebSocket communication in your projects, whether you are building a WebSocket server or client.

## Features

- Full WebSocket support (client and server)
- Lightweight and easy to use
- Compatible with **ESP8266** and **ESP32**
- Ideal for IoT applications that require real-time communication

## Installation

### 1. Via Arduino Library Manager

- Open **Arduino IDE**.
- Go to **Sketch > Include Library > Manage Libraries...**.
- Search for **Socketyee** in the library manager.
- Click **Install** to install the library.

### 2. Manual Installation

1. Download the latest release of the library from [GitHub](https://github.com/Faizyee/Socketyee).
2. Go to your Arduino libraries folder:
   - **Windows**: `Documents\Arduino\libraries`
   - **macOS**: `Documents/Arduino/libraries`
   - **Linux**: `~/Arduino/libraries`
3. Extract the downloaded `.zip` file and place it in the `libraries` folder.
4. Restart the Arduino IDE.

## Usage Example

### WebSocket Server Example

```cpp
#include <ESP8266WiFi.h>
#include <Socketyee.h>

// Replace with your network credentials
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

Socketyee server(8080);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  server.begin();
}

void loop() {
  server.loop();
  server.onMessage([](String message) {
    Serial.println("Received: " + message);
    server.send("Hello from ESP8266!");
  });
}
