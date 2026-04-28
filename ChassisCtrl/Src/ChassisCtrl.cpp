//
// Created by 34254 on 2026/2/12.
//

#include "../Inc/ChassisCtrl.h"
#include "../Inc/ChassisOnline.h"
#include "../Inc/ChassisOutline.h"
#include "Buzz.h"
static ChassisCtrl chassis_ctrl;
extern "C" {
#include "PID.h"
#include "grayscale_tracker.h"
  void chassis_ctrl_update() {
    chassis_ctrl.update();
  }
}
extern uint8_t choose_topic;
extern BUZZ buzz;

extern GRAY_Data data;
static ChassisOnline chassis_online(0.0f, 0.1f, 0.01f, 0.010f);
static ChassisOutline chassis_outline(0.0f, 0.1f, 0.01f, 0.010f);

ChassisCtrl::ChassisCtrl() {
}

void ChassisCtrl::set_strategy(ChassisStrategy *strategy) {
  if (strategy == &chassis_online && is_online_ == 0) {
    is_online_ = 1;
    current_num++;
  } else if (strategy == &chassis_outline && is_online_ == 1) {
    is_online_ = 0;
    current_num++;
  }
  strategy_ = strategy;
}

void ChassisCtrl::update() {
  switch (choose_topic) {
    case 1:
      target_num = 2;//经过两个间断点
      break;
    case 2:
      target_num = 3; //经过三个间断点
      break;
    case 3:
      target_num = 16; //跑四圈
      break;
    default:
      target_num = 0; //停止
      break;
  }

  set_mode();
  if (current_num < target_num) {
    strategy_->update();
  }else {
    PID_Clear(strategy_->speed_pid_);
    PID_Clear(strategy_->diff_pwm_pid_);
  }

}

void ChassisCtrl::set_mode() {
  if (data.digital != 0x00) {
    if (strategy_ != &chassis_online) {
      PID_Clear(strategy_->speed_pid_);
      PID_Clear(strategy_->diff_pwm_pid_);
    }
    set_strategy(&chassis_online);
  } else {
    if (strategy_ != &chassis_outline) {
      PID_Clear(strategy_->speed_pid_);
      PID_Clear(strategy_->diff_pwm_pid_);
    }
    set_strategy(&chassis_outline);
  }
}
