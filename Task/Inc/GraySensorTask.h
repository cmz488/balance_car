//
// Created by 34254 on 2026/4/10.
//

#ifndef BALANCE_CAR_GRAYSENSORTASK_H
#define BALANCE_CAR_GRAYSENSORTASK_H

#ifdef __cplusplus
#include "TaskBase.h"

extern "C" {
#endif
  void gray_sensor_task() ;
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class GraySensorTask : public TaskBase {
public:
  void run() override;
};
#endif


#endif //BALANCE_CAR_GRAYSENSORTASK_H
