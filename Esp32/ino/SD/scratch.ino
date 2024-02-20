//--------------Mic Dependencies-------------------

#include <driver/i2s.h>

#define I2S_SD 12
#define I2S_WS 15
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0

void i2s_install() {

  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
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

  const i2s_pin_config_t pin_config = {.bck_io_num = I2S_SCK,
                                       .ws_io_num = I2S_WS,
                                       .data_out_num = -1,
                                       .data_in_num = I2S_SD};

  i2s_set_pin(I2S_PORT, &pin_config);
}

//--------------General----------------

#define BUFFER_COUNT 10
#define AUDIO_BUFFER_LENGTH 1024 // audio buffer length ie. 1kb
int16_t sBuffer[AUDIO_BUFFER_LENGTH];

#define WAVFILE_SIZE                                                           \
  (1024 * 1) // no. of 1kb buffers (AUDIO_BUFFER_LENGTH) to be written

void printBuffer(const char *buffer, size_t length) {
  for (size_t i = 0; i < length; i++) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void main(void *parameters) {
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  size_t bytesIn = 0;
  int counter = 0;
  while (1) {
    esp_err_t result =
        i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);

    if (counter >= WAVFILE_SIZE) {
      counter = 0;
    }
    printBuffer((const char *)sBuffer, bytesIn);
    counter++;
  }
}

void setup() {
  Serial.begin(9600);
  micTask(NULL);
}

void loop() {}