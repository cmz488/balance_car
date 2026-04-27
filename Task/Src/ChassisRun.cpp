//
// Created by 34254 on 2026/2/12.
//

#include "../Inc/ChassisRun.h"
#include "cstring"
#include "BalanceCtrl.h"
#include "Buzz.h"
#define DT 0.001f
#include <math.h>

#include "usart.h"

#include "C:\Users\Administrator\Desktop\balance_car\Serial\Inc\Serial.h"

extern "C" {
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"
#include "LED.h"
#include "key.h"
ChassisRun chassisRun; // 创建 ChassisRun 任务实例
void ChassisRun_Task() {
  chassisRun.start(const_cast<char *>("chassisRun"), 256, osPriorityAboveNormal); // 启动 ChassisRun 任务
}
}

constexpr float speed_filter_alpha = 0.3f;
uint8_t runFlag = 0;
uint8_t keyNum = 0;
float left_speed;
float last_left_speed;
float right_speed;
float last_right_speed;
extern float pitch1;

extern int16_t g_raw[3];
extern BalanceCtrl balance_ctrl;
extern BUZZ buzz;
extern float g_real[3];
int16_t left_pwm, right_pwm;
extern Serial serial;
extern Serial balance_serial;

void ChassisRun::run() {
  for (;;) {
    keyNum = key_get_num();
    if (keyNum == 1) {
      runFlag = 1 - runFlag;
    }

    if (pitch1 > 50 || pitch1 < -50) {
      runFlag = 0;
    }
    if (runFlag) {
      LED_ON(0);
      LED_OFF(1);
      left_speed = Encoder_Get(1) / 44.0f / DT / 35.0f;
      right_speed = Encoder_Get(2) / 44.0f / DT / 35.0f;
      last_left_speed = left_speed;
      last_right_speed = right_speed;
      balance_ctrl.update(-(pitch1 - Angle_ki), left_speed - right_speed,
                          (left_speed + right_speed) / 2.0f * 2 * M_PI * 0.0325f, (left_speed + right_speed) / 2.0f,
                          DT);
      std::tie(left_pwm, right_pwm) = balance_ctrl.get_out_put();
      Motor_SetPWM(1, left_pwm);
      Motor_SetPWM(2, right_pwm);
    } else {
      Motor_SetPWM(1, 0);
      Motor_SetPWM(2, 0);
      LED_OFF(0);
      LED_ON(1);
    }
    osDelay(DT * 1000);
  }
}
