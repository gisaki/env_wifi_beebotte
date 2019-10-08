#include <M5StickC.h>

#include "lcd.h"
#include "lcd_icon.h"

// define
#define COLOR_RGB(red,green,blue) ( ((red>>3)<<11) | ((green>>2)<<5) | (blue>>3) )

// 各種インスタンス
TFT_eSprite M5_Img = TFT_eSprite(&M5.Lcd); // Sprite object
int tilt_ = 0;

// 
// 
// 
void lcd_setup() {
  // init
  M5.begin();

  // test
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextFont(FONT_8px);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("TEST");

  // Create a sprite for double buffering
  M5_Img.setColorDepth(8);
  M5_Img.createSprite(160, 80);
  M5_Img.fillSprite(BLACK);
  M5_Img.setTextColor(TFT_WHITE);
  M5_Img.setTextFont(FONT_8px);
  M5_Img.setTextSize(1);
}

void lcd_loop() {
  // loop
  M5.update();

  // double buffering
  // M5.Lcd.fillScreen(BLACK); // 
  M5_Img.pushSprite(0, 0);
  M5_Img.fillSprite(BLACK);

  // 背景
  {
    M5_Img.fillRect(
       0*8, (6*8 + 4 - 4) - 2, 
      20*8, (7*8 + 4 - 4) + 2, 
      COLOR_RGB(0x8Eu, 0x1Eu, 0xFFu)
    );
    M5_Img.fillRect(
       0*8, (7*8 + 4) - 2, 
      20*8, (9*8 + 4) + 2, 
      COLOR_RGB(0xCCu, 0xCCu, 0xFFu)
    );
  }

  lcd_draw_sensor_data();
  lcd_draw_icon_conn();
  lcd_draw_ip();
  lcd_draw_last();

}

void lcd_draw_sensor_data() {
  int16_t x = 0;
  int16_t y = 0;
  char buf[64];
  
  static int8_t pos_ = 0;
  pos_ = (pos_ + 3 + tilt_) % 3; // 0～2
  tilt_ = 0; // 反映させたらクリア

  float tmp = env_readTemperature();
  float hum = env_readHumidity();
  float pressure = env_readPressure();
  float vbat = powerctl_ReadVbat();

  {
    M5_Img.setTextFont(FONT_48px_NUM);
    x = 0*8 + 4;
    y = 0*8 + 4;
    M5_Img.setCursor(x, y);
    if (pos_ == 0) { snprintf(buf, sizeof(buf), "%.1f", tmp); }
    if (pos_ == 1) { snprintf(buf, sizeof(buf), "%.1f", hum); }
    if (pos_ == 2) { snprintf(buf, sizeof(buf), "%.0f", pressure / 100.0f); }
    M5_Img.print(buf);
    M5_Img.setTextFont(FONT_16px);
    if (pos_ == 0) { M5_Img.print("C"); }
    if (pos_ == 1) { M5_Img.print("%"); }
    M5_Img.setTextFont(FONT_8px);
    if (pos_ == 2) { M5_Img.print("hPa"); }
  }
  {
    x = 0*8 + 4;
    y = 6*8 + 4 - 4;
    M5_Img.setCursor(x, y);
    if (pos_ == 0) { snprintf(buf, sizeof(buf), "%.1f%%", hum); }
    if (pos_ == 1) { snprintf(buf, sizeof(buf), "%.1fhPa", pressure / 100.0f); }
    if (pos_ == 2) { snprintf(buf, sizeof(buf), "%.1fC", tmp); }
    M5_Img.print(buf);
    M5_Img.print(" ");
    if (pos_ == 0) { snprintf(buf, sizeof(buf), "%.1fhPa", pressure / 100.0f); }
    if (pos_ == 1) { snprintf(buf, sizeof(buf), "%.1fC", tmp); }
    if (pos_ == 2) { snprintf(buf, sizeof(buf), "%.1f%%", hum); }
    M5_Img.print(buf);
    M5_Img.print(" ");
    snprintf(buf, sizeof(buf), "%.3fV", vbat);
    M5_Img.print(buf);
  }
}

void lcd_settilt(int dir) {
    tilt_ = dir; // 本体の傾き 0: 水平、-1: 左傾き、1: 右傾き
    // 本関数は傾きが検知されたら呼ばれる。
    // tilt_ は表示に反映させたらクリアする
}

void lcd_draw_icon_conn() {
  int16_t x = 0*8 + 4;
  int16_t y = 7*8 + 4;
  if( wifi_connected() ) {
    if ( beebotte_connected() ) {
      // WiFi OK, MQTT OK
      // M5.Lcd.drawBitmap(x, y, 16, 16, icon_publish);
      M5_Img.pushImage(x, y, 16, 16, icon_publish);
    } else {
      // WiFi OK, MQTT NG
      // M5.Lcd.drawBitmap(x, y, 16, 16, icon_conn);
      M5_Img.pushImage(x, y, 16, 16, icon_conn);
    }
  } else {
    // WiFi NG
    // M5.Lcd.drawBitmap(x, y, 16, 16, icon_conn_ng);
    M5_Img.pushImage(x, y, 16, 16, icon_conn_ng);
  }
}

void lcd_draw_ip() {
  M5_Img.setTextColor( COLOR_RGB(0x1Eu, 0x1Eu, 0xFFu) );
  int16_t x = 3*8 + 4;
  int16_t y = 7*8 + 4;
  M5_Img.setCursor(x, y);
  M5_Img.println(WiFi.localIP());
  M5_Img.setTextColor(TFT_WHITE);
}

void lcd_draw_last() {
  M5_Img.setTextColor( COLOR_RGB(0x1Eu, 0x1Eu, 0xFFu) );

  int16_t x = 3*8 + 4;
  int16_t y = 8*8 + 4;

  char buf[64];
  beebotte_last_published(buf, sizeof(buf));

  M5_Img.setCursor(x, y);
  M5_Img.print("Lst:");
  M5_Img.println(buf);

  M5_Img.setTextColor(TFT_WHITE);
}
