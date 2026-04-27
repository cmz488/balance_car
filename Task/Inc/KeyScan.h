//
// Created by 34254 on 2026/2/11.
//

#ifndef PARADE_CAR_KEYSCAN_H
#define PARADE_CAR_KEYSCAN_H

#ifdef __cplusplus
#include "TaskBase.h"
extern "C" {
#endif
    void KeyScan_Task();
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class KeyScan : public TaskBase
{
public:
    void run() override; // 实现任务逻辑
};
#endif


#endif //PARADE_CAR_KEYSCAN_H