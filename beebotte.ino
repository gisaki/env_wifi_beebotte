#include <M5StickC.h>

#include <WiFiClientSecure.h>

// PubSubClientの標準の128byteでは不足するのでサイズを大きく指定
#define MQTT_MAX_PACKET_SIZE 1024
#include <PubSubClient.h>
// ↑上記ヘッダ内は以下のようになっており、先に任意の値を指定可能
// MQTT_MAX_PACKET_SIZE : Maximum packet size
// #ifndef MQTT_MAX_PACKET_SIZE
// #define MQTT_MAX_PACKET_SIZE 128
// #endif

#include "beebotte_certificate.h"
#include "beebotte_config.h"

// 
// define
// 
const char *host = "mqtt.beebotte.com";
#define _sp(x) Serial.print(x)
#define _sl(x) Serial.println(x)

// 
// 各種インスタンス
// 
WiFiClientSecure wifiClient;
PubSubClient client(host, 8883, wifiClient);

RTC_TimeTypeDef rtc_time_;
RTC_DateTypeDef rtc_date_;
unsigned long last_last_published_ms_;

// MQTTで subscribe した topic の受信時のコールバック関数
void callback(char* topic, byte* payload, unsigned int length) {
  // PubSubClient.hで定義されているMQTTの最大パケットサイズ
  char buffer[MQTT_MAX_PACKET_SIZE];

  snprintf(buffer, sizeof(buffer), "%s", payload);
  _sl("received:");
  _sp("topic: ");
  _sl(topic);
  _sl(buffer);
}

// MQTT connect および subscribe
void beebotte_connect_subscribe(int first) {
  // MQTT未接続の場合
  if (!client.connected()) {
    // ユーザ名を指定して接続
    String username = "token:";
    username += channelToken;
    client.connect(clientID, username.c_str(), NULL);

    if (client.connected()) {
      _sl("MQTT connected");
      if (first) { // 初回時のみ画面にも出力
        M5.Lcd.println("MQTT connected");
      }
      client.setCallback(callback);

      // トピック名を指定してsubscribe
      client.subscribe(topic_[0]);
    } else {
      _sp("MQTT connection failed: ");
      _sl(client.state());
      if (first) { // 初回時のみ画面にも出力
        M5.Lcd.print("MQTT connection failed: ");
        M5.Lcd.println(client.state());
      }
    }
  } else {
    // MQTT接続済みの場合は通常処理
    client.loop();
  }
}

// 
// 
// 
void beebotte_setup() {
    wifiClient.setCACert(test_ca_cert);
    
    // 接続
    beebotte_connect_subscribe(1);
}

void beebotte_loop() {
  // 通常処理、切断されていれば再接続
  beebotte_connect_subscribe(0);
}

void beebotte_publish(int idx, float value) {
  char buffer[MQTT_MAX_PACKET_SIZE];
  snprintf(buffer, sizeof(buffer), "{\"data\":%.3f, \"write\":true}", value);
  if (client.publish(topic_[idx], buffer)) {
    _sl("publish succeeded");
    _sl(buffer);
    // 最後に publish した時刻を控えておく
    M5.Rtc.GetTime(&rtc_time_);
    M5.Rtc.GetData(&rtc_date_);
    last_last_published_ms_ = millis();
  } else {
    _sl("publish failed");
    _sl(buffer);
  }
}

void beebotte_last_published(char *buf, int buflen) {

    // TODO NTPを使用するならば以下が活用できる
    // RTC_TimeTypeDef rtc_time_;
    // RTC_DateTypeDef rtc_date_;
    
    unsigned long now = millis();
    unsigned long diff = now - last_last_published_ms_;

#define SECOND_MS (1000)
#define MINUTE_MS (60 * SECOND_MS)
#define HOURS_MS (60 * MINUTE_MS)
#define DAY_MS (24 * HOURS_MS)

    if (diff < 1 * MINUTE_MS) {
        snprintf(buf, buflen, "%d sec ago", diff / SECOND_MS);
        return;
    } else if (diff < 1 * HOURS_MS) {
        snprintf(buf, buflen, "%d min ago", diff / MINUTE_MS);
        return;
    } else if (diff < 1 * DAY_MS) {
        snprintf(buf, buflen, "%d hours ago", diff / HOURS_MS);
        return;
    } else {
        snprintf(buf, buflen, "%d days ago", diff / DAY_MS);
        return;
    }
}

bool beebotte_connected() {
    if (client.connected()) {
        return true;
    }
    return false;
}
