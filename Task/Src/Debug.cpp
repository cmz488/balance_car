//
// Created by 34254 on 2026/2/12.
//

#include "../Inc/Debug.h"
#include "../serial/Inc/Serial.h"

extern "C" {
void Debug_Task() {
  static debug debugTask; // 创建 debug 任务实例
  debugTask.start((char *) "debugTask", 256, osPriorityNormal); // 启动 debug 任务
}
}
extern float angle_speed_kp;
extern float angle_speed_ki;
extern float angle_speed_kd;
extern float Angle_kp, Angle_ki, Angle_kd;
extern float Speed_kp, Speed_ki, Speed_kd;
extern float pitch;
extern float g[3];
extern float pitch1;
extern Serial serial;
extern int16_t left_pwm, right_pwm;

void debug::run() {
  serial.receive_init();
  serial.relocate({&Angle_kp, &Speed_kp,&angle_speed_kp}, {&Angle_ki, &Speed_ki,&angle_speed_ki}, {&Angle_kd, &Speed_kd,&angle_speed_kd});
  for (;;) {
    serial.write(
      Angle_kp, Angle_ki, Angle_kd, Speed_kp, Speed_ki, Speed_kd,angle_speed_kp,angle_speed_ki,angle_speed_kd, pitch1,left_pwm, right_pwm
    );
    osDelay(10);
  }
}
