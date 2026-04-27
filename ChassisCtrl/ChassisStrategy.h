//
// Created by 34254 on 2026/4/10.
//

#ifndef BALANCE_CAR_CHASSISSTATEGY_H
#define BALANCE_CAR_CHASSISSTATEGY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "PID.h"
extern PID_t TurnPID;
  extern PID_t SpeedPID;
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class ChassisStrategy {
public:
  PID_t *speed_pid_{&SpeedPID};
  PID_t *diff_pwm_pid_{&TurnPID};
  virtual ~ChassisStrategy() = default;
  virtual void update() = 0;
};
#endif


#endif //BALANCE_CAR_CHASSISSTATEGY_H