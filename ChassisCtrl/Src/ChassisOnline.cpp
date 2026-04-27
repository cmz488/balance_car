//
// Created by 34254 on 2026/4/15.
//

#include "../Inc/ChassisOnline.h"
#include <cstdint>

extern "C" {
}

extern int16_t gray_offset;
extern uint8_t gray_digital;

ChassisOnline::ChassisOnline(float target_speed, float kp, float ki, float kd) : target_speed_(target_speed) {
  PID_Init(&turn_angle_pid_);
  turn_angle_pid_.Kp = kp;
  turn_angle_pid_.Ki = ki;
  turn_angle_pid_.Kd = kd;
}

void ChassisOnline::update() {
    turn_angle_pid_.Target = 0.0f;
    turn_angle_pid_.Actual = static_cast<float>(gray_offset);
    PID_Update(&turn_angle_pid_);
    speed_pid_->Target = target_speed_;
    diff_pwm_pid_->Target =turn_angle_pid_.Out;
}

