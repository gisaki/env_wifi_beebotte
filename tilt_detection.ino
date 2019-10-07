#include <M5StickC.h>

int dir_ = 0; // 本体の傾き 0: 水平、-1: 左傾き、1: 右傾き

// 
// 
// 
void tilt_detection_setup() {
  M5.MPU6886.Init();
}

void tilt_detection_loop() {

    static long loopTime_, startTime_ = 0;
    loopTime_ = millis();
    if(startTime_ < (loopTime_ - 50)){
        startTime_ = loopTime_;

        // 加速度から左右に傾けたかどうかを判定する
        float accX, accY, accZ;
        M5.MPU6886.getAccelData(&accX,&accY,&accZ);
        
        // accY が長い辺の傾きに該当
        // ラフに判定（本当は加速度を積算するといいのだろうけれど）
        if (accY > 0.5)  { dir_ =  1; } // 右傾き
        if (accY < -0.5) { dir_ = -1; } // 左傾き
    }
}

int tilt_detection_getdir() {
    return dir_;
}
void tilt_detection_resetdir() {
    dir_ = 0;
}
