// 
// 各種機能が提供するAPIという位置づけ
// 

extern float powerctl_ReadVbat();

extern int tilt_detection_getdir();
extern void tilt_detection_resetdir();

extern void beebotte_publish(int idx, float value);
extern void beebotte_last_published(char *buf, int buflen);
extern bool beebotte_connected();

extern float env_readTemperature();
extern float env_readHumidity();
extern float env_readPressure();

extern bool wifi_connected();

extern void lcd_settilt(int dir);
