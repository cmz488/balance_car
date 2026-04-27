//
// Created by 34254 on 2026/4/10.
//

#include "../Inc/GraySensorTask.h"
#include "GraySensor.h"
static GraySensorTask gray_sensor_task_instance;
extern "C" {
  void gray_sensor_task() {
    gray_sensor_task_instance.start((char*)"gray_sensor_task",256,osPriorityNormal);
  }
}
int16_t todo_rename;
uint8_t digital;
void GraySensorTask::run() {
  GraySensor* gray_sensor_instance = get_gray_sensor_intance();
  for (;;) {
    todo_rename = gray_sensor_instance->get_offset();
    digital = gray_sensor_instance->get_digital();
    osDelay(20);
  }
}
