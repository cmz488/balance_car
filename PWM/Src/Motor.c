#include "main.h"                  // Device header
#include "PWM.h"
#include "Motor.h"
void Motor_Init(void)
{
	PWM_Init();
}
/**
 @brief 设置电机PWM占空比
 @param n ---电机编号1：左电机，2：右电机
 @param PWM ---占空比，正数表示正转
**/
void Motor_SetPWM(uint8_t n, int16_t PWM)
{
	if (n==1) {
		if (PWM >= 0) //左电机正转
		{
			HAL_GPIO_WritePin(GPIOB, AIN_1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, AIN_2_Pin, GPIO_PIN_RESET);
			PWM_SetCompare1(PWM);
		}
		else //左电机反转
		{
			HAL_GPIO_WritePin(GPIOB, AIN_2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, AIN_1_Pin, GPIO_PIN_RESET);
			PWM_SetCompare1(-PWM);
		}
	}else if (n==2) {
		if (PWM >= 0) //右电机正转
		{
			HAL_GPIO_WritePin(GPIOD, BIN_1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, BIN_2_Pin, GPIO_PIN_SET);
			PWM_SetCompare2(PWM);
		}
		else //右电机反转
		{
			HAL_GPIO_WritePin(GPIOD, BIN_2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, BIN_1_Pin, GPIO_PIN_SET);
			PWM_SetCompare2(-PWM);
		}
	}

}
