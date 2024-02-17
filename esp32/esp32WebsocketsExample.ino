#include <WiFi.h>
#include <WebSocketsClient.h>

const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";
const char *webSocketServerAddress = "wss://echo.websocket.org"; // Replace with your WebSocket server address
const uint16_t port = 8765; // Replace with your WebSocket server port

WebSocketsClient webSocket;

void setup()
{
    Serial.begin(9600);
    delay(1000);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");

    webSocket.begin(webSocketServerAddress , port , "/");
    webSocket.onEvent(webSocketEvent);
}

void loop()
{
    webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("WebSocket disconnected");
        break;
    case WStype_CONNECTED:
        Serial.println("WebSocket connected");
        break;
    case WStype_TEXT:
        Serial.print("Received message: ");
        Serial.println((char *)payload);
        break;
    case WStype_ERROR:
        Serial.print("WebSocket error: ");
        Serial.println(webSocket.lastError());
        break;
    }
}