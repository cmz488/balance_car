//
// Created by 34254 on 2026/2/12.
//

#include "../Inc/ChassisCtrl.h"
#include "Buzz.h"
static ChassisCtrl chassis_ctrl;
extern "C" {
#include "PID.h"
  void chassis_ctrl_update() {
    chassis_ctrl.update();
  }
}

extern BUZZ buzz;
extern int16_t gray_offset;
extern uint8_t gray_digital;
static ChassisOnline chassis_online(10.0f, 0.5f, 0.1f, 0.01f);
static ChassisOutline chassis_outline(10.0f, 0.0f, 1.0f, 0.0f, 0.0f);

ChassisCtrl::ChassisCtrl() {
}

void ChassisCtrl::set_strategy(ChassisStrategy *strategy) {
  if (strategy == &chassis_online && is_online_ == 0) {
    is_online_ = 1;
    buzz.ring();
  } else if (strategy == &chassis_outline && is_online_ == 1) {
    is_online_ = 0;
    buzz.ring();
  }
  strategy_ = strategy;
}

void ChassisCtrl::update() {
  set_mode();
  strategy_->update();
}

void ChassisCtrl::set_mode() {
  if (gray_digital != 0x00) {
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
