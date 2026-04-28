//
// Created by 34254 on 2026/4/15.
//

#include "../Inc/ChassisOutline.h"
#include <cmath>

extern "C" {
}
extern  float Yaw;
extern uint8_t outline_move_strategy_flag;

ChassisOutline::ChassisOutline(float target_speed, float kp, float ki,
                               float kd) : target_speed_(target_speed) {
  PID_Init(&turn_angle_pid_);
  turn_angle_pid_.Kp = kp;
  turn_angle_pid_.Ki = ki;
  turn_angle_pid_.Kd = kd;
}

void ChassisOutline::update() {
  setOutlineMoveStrategy(static_cast<OutlineMoveStrategy>(outline_move_strategy_flag));
  switch (outline_move_strategy) {
    case CROSS:
      if (is_first_outline_) {
        turn_angle_pid_.Target = Yaw-30.0f;
        is_first_outline_ = 0;
      }
      turn_angle_pid_.Actual = Yaw;
      PID_Update(&turn_angle_pid_);
      diff_pwm_pid_->Target = turn_angle_pid_.Out;
      if (fabsf(Yaw - turn_angle_pid_.Target) < 3.0f) {
        speed_pid_->Target = target_speed_;
        is_first_outline_ = 1;
      } else {
        speed_pid_->Target = 0.0f;
      }
      break;
    case STRAIGHT:
      turn_angle_pid_.Target = 0.0f;
      speed_pid_->Target = target_speed_;
      diff_pwm_pid_->Target = 0;
      break;
    default:
      break;
  }
}
