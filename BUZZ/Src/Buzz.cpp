//
// Created by 34254 on 2026/4/15.
//

#include "../Inc/Buzz.h"

BUZZ buzz;
extern "C" {
}

BUZZ::BUZZ() {
}

void BUZZ::ring() {
  HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, GPIO_PIN_SET);
}
