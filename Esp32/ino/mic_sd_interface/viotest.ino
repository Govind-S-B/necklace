#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SD_MMC.h>
#include <SPI.h>
#include <driver/i2s.h>

struct wavHeader {
  char chunkId[4];
  uint32_t chunkSize;
  char format[4];
  char subchunk1Id[4];
  uint32_t subchunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  char subchunk2Id[4];
  uint32_t subchunk2Size;

  wavHeader(uint32_t sampleRate, uint16_t bitsPerSample, uint16_t numChannels,
            uint32_t dataSize) {
    memcpy(chunkId, "RIFF", 4);
    chunkSize = dataSize + 36;
    memcpy(format, "WAVE", 4);
    memcpy(subchunk1Id, "fmt ", 4);
    subchunk1Size = 16;
    audioFormat = 1;
    this->numChannels = numChannels;
    this->sampleRate = sampleRate;
    byteRate = sampleRate * numChannels * (bitsPerSample / 8);
    blockAlign = numChannels * (bitsPerSample / 8);
    this->bitsPerSample = bitsPerSample;
    memcpy(subchunk2Id, "data", 4);
    subchunk2Size = dataSize;
  }
};

#define I2S_NUM I2S_NUM_0
#define SAMPLE_RATE 16000
#define SAMPLE_SIZE 16
#define CHANNELS 1
#define RECORD_TIME 50000 // 5 minutes in milliseconds
#define BUFFER_SIZE 1024

// I2S pins
#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 14

uint32_t fileCounter = 0;

String getNextFileName() {
  String fileName = "/audio_" + String(fileCounter) + ".wav";
  fileCounter++;
  return fileName;
}

void setup() {
  Serial.begin(9600);

  // Mount the built-in SD card
  if (!SD_MMC.begin()) {
    Serial.println("Failed to mount SD card");
    return;
  }

  // Configure I2S
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = (i2s_bits_per_sample_t)SAMPLE_SIZE,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format =
          (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 2,
      .dma_buf_len = BUFFER_SIZE,
  };

  // Initialize I2S
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, NULL);
  i2s_set_clk(I2S_NUM, SAMPLE_RATE, (i2s_bits_per_sample_t)SAMPLE_SIZE,
              (i2s_channel_t)CHANNELS);

  // Start recording
  Serial.println("Recording started...");
  uint32_t startTime = millis();
  uint32_t bytesRead = 0;
  uint8_t buffer[BUFFER_SIZE];

  String fileName = getNextFileName();
  File file = SD_MMC.open(fileName.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  // Write WAV header
  uint32_t dataSize =
      (SAMPLE_SIZE / 8) * SAMPLE_RATE * CHANNELS * (RECORD_TIME / 1000);
  wavHeader header = wavHeader(SAMPLE_RATE, SAMPLE_SIZE, CHANNELS, dataSize);
  file.write((const uint8_t *)&header, sizeof(wavHeader));

  while (1) {
    // Read audio samples from the I2S bus
    i2s_read(I2S_NUM, buffer, BUFFER_SIZE, &bytesRead, portMAX_DELAY);

    // Save the samples to the SD card
    file.write(buffer, bytesRead);

    // Check if 5 minutes have passed
    if (millis() - startTime >= RECORD_TIME) {
      file.close();
      Serial.println("Recording finished.");

      // Start a new recording
      startTime = millis();
      fileName = getNextFileName();
      file = SD_MMC.open(fileName.c_str(), FILE_WRITE);
      if (!file) {
        Serial.println("Failed to open file for writing");
        break;
      }

      // Write WAV header
      file.write((const uint8_t *)&header, sizeof(wavHeader));
      Serial.println("Recording started...");
    }
  }

  // Clean up
  i2s_driver_uninstall(I2S_NUM);
}

void loop() {
  // Nothing to do
}
