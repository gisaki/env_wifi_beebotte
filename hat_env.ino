/*
    note: need add library Adafruit_BMP280 from library manage
    Github: https://github.com/adafruit/Adafruit_BMP280_Library
*/

#include <M5StickC.h>
#include "DHT12.h"
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>

// 制約事項：
// ひとまず BMM150 は使用しないことにする

// define
#define _sp(x) Serial.print(x)
#define _sl(x) Serial.println(x)

// 各種インスタンス
DHT12 dht12; 
Adafruit_BMP280 bme;

uint8_t dht12_init_done_ = 0;
uint8_t bme_init_done_ = 0;

// 
// 
// 
void env_setup() {
  Wire.begin(0,26);

  // DHT12
  // 特になし
  dht12_init_done_ = 1;

  // BMP280
  if (!bme.begin(0x76)){  
    _sl("Could not find a valid BMP280 sensor, check wiring!");
    bme_init_done_ = 0;
  } else {
    bme_init_done_ = 1;
    _sl("Initialize done!");
  }
}

void env_loop() {
  // BMP280
  if (!bme.begin(0x76)){  
    _sl("Could not find a valid BMP280 sensor, check wiring!");
    bme_init_done_ = 0;
  } else {
    bme_init_done_ = 1;
    _sl("Initialize done!");
  }
}

float env_readTemperature() {
  return dht12.readTemperature();
}

float env_readHumidity() {
  return dht12.readHumidity();
}

float env_readPressure() {
  if (bme_init_done_) {
    return bme.readPressure();
  }
  return 0.0f;
}
