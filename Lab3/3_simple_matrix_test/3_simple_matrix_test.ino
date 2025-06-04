#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_Config config;

void setup() {
  Serial.begin(115200);
  config.mx_width = 64;
  config.mx_height = 64;
  config.chain_length = 1;
  Serial.println("✅ MatrixPanel_Config is working.");
}

void loop() {}
