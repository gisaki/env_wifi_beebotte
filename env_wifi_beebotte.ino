#include <M5StickC.h>
#include "env_wifi_beebotte.h"

// define
#define SEC_TO_SLEEP  (8)   /* Time ESP32 will go to sleep (in seconds) */
#define CYC_MS_LCD_REPAINT (1000) /* 画面の表示更新周期 ミリ秒 */
#define CYC_MS_SENDOR_SEND (5 * 60 * 1000) /* センサデータの送信周期 ミリ秒 */

// 各種インスタンス
char always_up_mode_ = 0;

void setup(){
  // init
  Serial.begin(115200);
  pinMode(M5_BUTTON_HOME, INPUT);
  pinMode(M5_BUTTON_RST, INPUT);

  M5.Axp.ScreenBreath(12); // 12がmaxのはず
  // M5Stackでは M5.Lcd.setBrightness() だが、M5StickC では M5.Axp.ScreenBreath() に変更されている

  // 常時起動か？（起動時のHOMEボタンONで有効）
  always_up_mode_ = (digitalRead(M5_BUTTON_HOME) == LOW) ? 1 : 0;

  powerctl_setup();
  tilt_detection_setup();
  lcd_setup();
  wifi_setup();
  env_setup();
  beebotte_setup();
}

void loop() {
  static long loopTime_, startTime_ = 0;
  static int cnt_pub_ = 0;
  static int cnt_lcd_ = 0;

  loopTime_ = millis();
  if(startTime_ < (loopTime_ - CYC_MS_LCD_REPAINT)){
    startTime_ = loopTime_;
    
    Serial.println("Hello, World!");

    powerctl_loop();
    lcd_loop();
    wifi_loop();
    env_loop();
    beebotte_loop();

    // センサデータ送信
    float tmp = env_readTemperature();
    float hum = env_readHumidity();
    float pressure = env_readPressure();
    float vbat = powerctl_ReadVbat();
    if (cnt_pub_ == 0) {
      beebotte_publish(0, tmp);
      beebotte_publish(1, hum);
      beebotte_publish(2, pressure);
      beebotte_publish(3, vbat);
    }
    cnt_pub_ = ( cnt_pub_ + 1 ) % (CYC_MS_SENDOR_SEND / CYC_MS_LCD_REPAINT);

    if (always_up_mode_) {
      if (cnt_lcd_ >= SEC_TO_SLEEP) {
        M5.Axp.ScreenBreath(7);
        cnt_lcd_ = 0; // 念のため周期的に画面を暗くするため
      }
    } else {
      // 
      // https://lang-ship.com/reference/unofficial/M5StickC/Class/AXP192/
      // 
      
      // 上位から順に切断して、最後に deep sleep する
      beebotte_disconnect();
      wifi_disconnect();
      long time_in_ms = max(CYC_MS_SENDOR_SEND - loopTime_, (long)(CYC_MS_SENDOR_SEND / 5)); // 頻繁に起動しないように、最短でも 1/5 はsleepさせる
      M5.Axp.DeepSleep( SLEEP_MSEC(time_in_ms) );
      
      // deep sleep 以降までのダミーの待ち時間（適当）
      delay(1000);
    }
    
    cnt_lcd_ = ( cnt_lcd_ + 1 );
  }

  // 傾き検出は常時実施する（中で周期的に実施している）
  tilt_detection_loop();
  int dir = tilt_detection_getdir();
  if ( (dir == -1) || (dir == 1) ) {
    Serial.print("Tilt Detect: ");
    Serial.println(dir);
    lcd_settilt(dir); // LCDの表示をローテーション
    tilt_detection_resetdir();
  }

  // HOMEボタン　→画面復帰
  if(digitalRead(M5_BUTTON_HOME) == LOW){
    M5.Axp.ScreenBreath(12); // 12がmaxのはず
    cnt_lcd_ = 0;
    // M5Stackでは M5.Lcd.setBrightness() だが、M5StickC では M5.Axp.ScreenBreath() に変更されている
    while(digitalRead(M5_BUTTON_HOME) == LOW) {;}
  }

  // RSTボタン　→publish
  if(digitalRead(M5_BUTTON_RST) == LOW){
    // スイッチを押したタイミングで publish を発行するように
    cnt_pub_ = 0;
    // 何回か実行されてもよいので、チャタリングは気にしない（影響がない）
    while(digitalRead(M5_BUTTON_RST) == LOW) {;}
  }

}
