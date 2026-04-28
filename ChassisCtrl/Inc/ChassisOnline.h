//
// Created by 34254 on 2026/4/15.
//

#ifndef BALANCE_CAR_CHASSISONLINE_H
#define BALANCE_CAR_CHASSISONLINE_H

#ifdef __cplusplus
#include "ChassisStrategy.h"
extern "C" {
#endif
#include "main.h"
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class ChassisOnline : public ChassisStrategy{
  float target_speed_;
  PID_t turn_angle_pid_;
public:
  ChassisOnline(float target_speed,float kp ,float ki ,float kd);
  ~ChassisOnline() override = default;
  void update() override;
};
#endif


#endif //BALANCE_CAR_CHASSISONLINE_H