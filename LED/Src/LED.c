//
// Created by 34254 on 2026/2/11.
//

#include "../Inc/LED.h"
#include  "main.h"
//pf9 led0
//pf10 led1
void LED_ON(uint8_t LED_id) {
  if (LED_id == 0) {
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
  } else if (LED_id == 1) { HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET); }
}

void LED_OFF(uint8_t LED_id) {
  if (LED_id == 0) {
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
  } else if (LED_id == 1) { HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET); }
}

void LED_Toggle(uint8_t LED_id) {
  if (LED_id == 0) {
    HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
  } else if (LED_id == 1) { HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10); }
}
