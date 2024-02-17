#include <WiFi.h>
#include <WebSocketsClient.h>
#include <driver/i2s.h>

const char *ssid = "Aetos";
const char *password = "12345679";
const char *webSocketServerAddress = "192.168.28.180"; // Replace with your WebSocket server address
const uint16_t port = 8765;                            // Replace with your WebSocket server port

WebSocketsClient webSocket;

// MIC CODE
#define I2S_SD 12
#define I2S_WS 15
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0

#define bufferCnt 10
#define bufferLen 1024
int16_t sBuffer[bufferLen];

bool isWebSocketConnected = false;

void i2s_install()
{
    // Set up I2S Processor configuration
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 44100,
        //.sample_rate = 16000,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = 0,
        .dma_buf_count = bufferCnt,
        .dma_buf_len = bufferLen,
        .use_apll = false};

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin()
{
    // Set I2S pin configuration
    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD

            i2s_set_pin(I2S_PORT, &pin_config);
}
}

void micTask(void *parameter, WebSocketsClient client)
{

    i2s_install();
    i2s_setpin();
    i2s_start(I2S_PORT);

    size_t bytesIn = 0;
    while (1)
    {
        esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
        if (result == ESP_OK && isWebSocketConnected)
        {
            webSocket.sendBinary((const char *)sBuffer, bytesIn);
        }
    }
}

void setupWiFi()
{
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void setup()
{
    Serial.begin(9600);
    setupWiFi() // wifi setup
                // websocket setup
        webSocket.begin(webSocketServerAddress, port, "/");
    webSocket.onEvent(webSocketEvent);
    xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
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
        isWebSocketConnected = false;
        break;
    case WStype_CONNECTED:
        Serial.println("WebSocket connected");
        isWebSocketConnected = true;
        break;
    case WStype_TEXT:
        Serial.print("Received message: ");
        Serial.println((char *)payload);
        break;
    case WStype_ERROR:
        Serial.print("WebSocket error: ");
        break;
    }
}