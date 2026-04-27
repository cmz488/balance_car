//
// Created by 34254 on 2026/2/12.
//

#ifndef PARADE_CAR_MPU6050TASK_H
#define PARADE_CAR_MPU6050TASK_H

#ifdef __cplusplus
#include "TaskBase.h"
extern "C" {
#endif
void MPU6050_Task();
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class MPU6050Task :public TaskBase{
public:
    void run() override;
};
#endif


#endif //PARADE_CAR_MPU6050TASK_H