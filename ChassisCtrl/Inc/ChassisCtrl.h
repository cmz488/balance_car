//
// Created by 34254 on 2026/2/12.
//

#ifndef PARADE_CAR_CHASSISCTRL_H
#define PARADE_CAR_CHASSISCTRL_H
#pragma once
#include "main.h"
#ifdef __cplusplus
#include "ChassisStrategy.h"
#include "ChassisOnline.h"
#include "ChassisOutline.h"
extern "C" {
#endif
  void chassis_ctrl_update();
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class ChassisCtrl {
  ChassisStrategy *strategy_{nullptr};
  uint8_t is_online_{0};

public:
  ChassisCtrl();
  void set_strategy(ChassisStrategy *strategy);
  void set_mode();
  void update() ;
};
#endif


#endif //PARADE_CAR_CHASSISCTRL_H
