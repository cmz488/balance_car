//
// Created by 34254 on 2026/4/14.
//

#include "../Inc/BalanceCtrl.h"

BalanceCtrl balance_ctrl;
extern "C" {
}

BalanceCtrl::BalanceCtrl() : angle_speed_pid_(AnglePID(angle_speed_kp,angle_speed_ki,angle_speed_kd,2000,1000)),
                              pitch_pid_(SpeedPID(Angle_kp, 0.0f, Angle_kd, 2000, 1000)),
                             speed_pid_(SpeedPID(Speed_kp, Speed_ki, Speed_kd, 20, 10)),
                             turn_pid_(SpeedPID(0, 0, 0, 0, 0)),
                             target_speed_(0), left_output_(0), right_output_(0) {
}

void BalanceCtrl::update(const float pitch, const float diff_speed, const float average_speed, const float gyro, float dt) {
  angle_speed_pid_.AnglePID_set(angle_speed_kp,angle_speed_ki,angle_speed_kd);
  pitch_pid_.SpeedPID_set(Angle_kp, 0, Angle_kd);
  speed_pid_.SpeedPID_set(Speed_kp, Speed_ki, Speed_kd);
  float speed_output = speed_pid_.speed_PID_calculate(target_speed_, average_speed, dt);
  float angle_output = pitch_pid_.speed_PID_calculate(speed_output, pitch, dt);
  float angle_speed_output = angle_speed_pid_.AnglePID_Calculate(angle_output,gyro,dt);
  float diff_output = turn_pid_.speed_PID_calculate(0, diff_speed, dt); // TODO! 脱线状态下应该以yaw角度环为外环
  left_output_ = std::clamp(angle_speed_output - diff_output/2,-2000.0f,2000.0f);
  right_output_ = std::clamp(angle_speed_output + diff_output/2,-2000.0f,2000.0f);
}

void BalanceCtrl::clear() {
  left_output_ = 0;
  right_output_ = 0;
  speed_pid_.speed_PID_clear();
  turn_pid_.speed_PID_clear();
  pitch_pid_.speed_PID_clear();
  angle_speed_pid_.clear();
}