//
// Created by 34254 on 2026/4/14.
//

#ifndef BALANCE_CAR_BALANCECTRL_H
#define BALANCE_CAR_BALANCECTRL_H


#ifdef __cplusplus
#include <cstdint>
#include <tuple>
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
inline float Angle_kp=0.0f;
inline float Angle_ki=0.0f;
inline float Angle_kd=0.0f;

inline float Speed_kp=0.0f;
inline float Speed_ki=0.0f;
inline float Speed_kd=0.0f;

inline float angle_speed_kp=0.0f;
inline float angle_speed_ki=0.0f;
inline float angle_speed_kd=0.0f;
class BalanceCtrl {
  AnglePID angle_speed_pid_; //最内环 角速度环
  SpeedPID pitch_pid_; //中间 直立环
  SpeedPID speed_pid_; //外环 速度环
  SpeedPID turn_pid_; //差速环控制转向

  float target_speed_;

  int16_t left_output_;
  int16_t right_output_;

public:
  BalanceCtrl();

  ~BalanceCtrl() = default;
  void update(float pitch, float diff_speed, float average_speed, float gyro, float dt);
  void clear();
  int16_t getLeftOutput() const { return left_output_; }
  int16_t getRightOutput() const { return right_output_; }

  auto get_out_put()->std::tuple<int16_t,int16_t>{return std::make_tuple(left_output_,right_output_);}
};
#endif


#endif //BALANCE_CAR_BALANCECTRL_H
