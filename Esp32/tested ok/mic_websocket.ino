#include <ArduinoWebsockets.h> //"WebSockets" by Markus Sattler
#include <WiFi.h>
#include <driver/i2s.h>

#define I2S_SD 13
#define I2S_WS 15
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0

#define bufferCnt 10
#define bufferLen 1024
int16_t sBuffer[bufferLen];

const char *ssid = "Aetos";        //<WIFI_SSID>
const char *password = "12345679"; //<WIFI_PASSWORD>

const char *websocket_server_host = "139.84.139.39"; //<WEBSOCKET_SERVER_HOST>
const uint16_t websocket_server_port = 8888;         // <WEBSOCKET_SERVER_PORT>

using namespace websockets;
WebsocketsClient client;
bool isWebSocketConnected;

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
    isWebSocketConnected = true;
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
    isWebSocketConnected = false;
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

void i2s_install() {
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      // .sample_rate = 44100,
      .sample_rate = 16000,
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0,
      .dma_buf_count = bufferCnt,
      .dma_buf_len = bufferLen,
      .use_apll = false};

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {.bck_io_num = I2S_SCK,
                                       .ws_io_num = I2S_WS,
                                       .data_out_num = -1,
                                       .data_in_num = I2S_SD};

  i2s_set_pin(I2S_PORT, &pin_config);
}

void setup() {
  Serial.begin(9600);

  connectWiFi();
  connectWSServer();
  xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
}

void loop() {}

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void connectWSServer() {
  client.onEvent(onEventsCallback);
  while (!client.connect(websocket_server_host, websocket_server_port, "/")) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Websocket Connected!");
}

void micTask(void *parameter) {
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  size_t bytesIn = 0;
  while (1) {
    esp_err_t result =
        i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
    if (result == ESP_OK && isWebSocketConnected) {
      client.sendBinary((const char *)sBuffer, bytesIn);
    }
  }
}