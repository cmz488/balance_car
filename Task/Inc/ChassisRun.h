//
// Created by 34254 on 2026/2/12.
//

#ifndef PARADE_CAR_CHASSISRUN_H
#define PARADE_CAR_CHASSISRUN_H

#ifdef __cplusplus
#include "TaskBase.h"
extern "C" {
#endif
void ChassisRun_Task();
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class ChassisRun:public TaskBase {
public:
    void run() override;
};
#endif


#endif //PARADE_CAR_CHASSISRUN_H