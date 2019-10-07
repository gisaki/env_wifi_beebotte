#include <M5StickC.h>

// 
// 
// 
void powerctl_setup() {
  // 特になし
}

void powerctl_loop() {
  // 特になし
}

float powerctl_ReadVbat() {
  // 1.1倍は根拠はよくわからない。
  // サンプルスケッチで以下で計算していたので、数式はそのまま利用。
  return (M5.Axp.GetVbatData() * 1.1f / 1000.0f);
}
