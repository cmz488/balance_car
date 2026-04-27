//
// Created by 34254 on 2026/2/11.
//

#include "../Inc/KeyScan.h"
#define DT 0.01f

extern "C" {
#include "key.h"


static KeyScan keyScan; // 创建 KeyScan 任务实例
    void KeyScan_Task() {
        keyScan.start((char*)"keyscan", 128, osPriorityNormal); // 启动 KeyScan 任务
    }
}

void KeyScan::run() {
    for (;;) {
        key_scan();
        osDelay(DT * 1000);
    }
}