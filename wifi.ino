#include <M5StickC.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include "wifi_config.h"

// define
#define _sp(x) Serial.print(x)
#define _sl(x) Serial.println(x)
#define WIFI_TIMEOUT (5000)

// 各種インスタンス
WiFiMulti wifiMulti;
uint8_t wifi_connected_ = 0;

// 
// 
// 
void wifi_setup() {
    wifiMulti.addAP(SSID1, PASS1);
    wifiMulti.addAP(SSID2, PASS2);
    wifiMulti.addAP(SSID3, PASS3);

    _sl("Connecting Wifi...");
    if(wifiMulti.run(WIFI_TIMEOUT) == WL_CONNECTED) {
        wifi_connected_ = 1;
        _sl("");
        _sl("WiFi connected");
        _sl("IP address: ");
        _sl(WiFi.localIP());
        M5.Lcd.println("");
        M5.Lcd.println("WiFi connected");
        M5.Lcd.println("IP address: ");
        M5.Lcd.println(WiFi.localIP());
    } else {
        wifi_connected_ = 0;
        _sl("WiFi not connected!");
        M5.Lcd.println("WiFi not connected!");
    }
}

void wifi_loop() {
    if(wifiMulti.run(WIFI_TIMEOUT) != WL_CONNECTED) {
        wifi_connected_ = 0;
        _sl("WiFi not connected!");
    } else {
        wifi_connected_ = 1;
    }
}

bool wifi_connected() {
    return (wifi_connected_)? true: false;
}
