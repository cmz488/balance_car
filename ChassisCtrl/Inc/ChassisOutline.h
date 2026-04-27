//
// Created by 34254 on 2026/4/15.
//

#ifndef BALANCE_CAR_CHASSISOUTLINE_H
#define BALANCE_CAR_CHASSISOUTLINE_H

#ifdef __cplusplus
#include "ChassisStrategy.h"

extern "C" {
#endif
#include "main.h"
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
enum OutlineMoveStrategy: uint8_t {
  CROSS = 0,
  STRAIGHT = 1,
};

class ChassisOutline : public ChassisStrategy {
  OutlineMoveStrategy outline_move_strategy;
  float target_speed_;
  float diff_speed_;
  PID_t turn_angle_pid_{};
  uint8_t is_first_outline_{1};
public:
  ChassisOutline(float target_speed, float diff_speed,float kp,float ki,float kd);
  ~ChassisOutline() override = default;
  void update() override;
  void setOutlineMoveStrategy(OutlineMoveStrategy strategy) {outline_move_strategy = strategy;}
};
#endif


#endif //BALANCE_CAR_CHASSISOUTLINE_H
