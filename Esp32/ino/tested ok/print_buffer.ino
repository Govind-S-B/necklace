#include "SD_MMC.h"
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <driver/i2s.h>

#define I2S_SD 13
#define I2S_WS 15
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0

#define bufferCnt 10
#define bufferLen 1024
int16_t sBuffer[bufferLen];

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

void clearAll() {
  if (SD_MMC.remove("/samples/")) {
    Serial.println("samples deleted cleared");
  } else {
    Serial.println("samples deletion failed");
  }
}

void writeSample(uint16_t *data, String filename) {
  File file = SD_MMC.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("File create failed");
    return;
  }
  file.write(*data);
  file.close();
  Serial.println("File " + filename + " write successful");
}

void setupSD() {
  if (!SD_MMC.begin()) {
    Serial.println("SD Card initialization failed!");
    return;
  }

  if (SD_MMC.exists("/samples/")) {
    Serial.println("folder exists");
  } else {
    Serial.println("File does not exist");
    if (SD_MMC.mkdir("/samples/")) {
      Serial.println("samples directory created");
    } else {
      Serial.println("samples directory creation failed");
    }
  }
}

void setup() {
  Serial.begin(9600);
  setupSD();
  const char *payload = "hey this is bulletlaunch";
  uint16_t number = (uint16_t)strtoul(payload, NULL, 10);
  writeSample(&number, "sample.txt");

  // xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
  micTask(NULL);
}

void loop() {}

void printBuffer(const char *buffer, size_t length) {
  for (size_t i = 0; i < length; i++) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void micTask(void *parameters) {

  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  size_t bytesIn = 0;
  while (1) {
    esp_err_t result =
        i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
    printBuffer((const char *)sBuffer, bytesIn);
  }
}