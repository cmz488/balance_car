#include "main.h"                  // Device header
#include "tim.h"
void PWM_Init(void)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void PWM_SetCompare1(uint16_t Compare)
{
	//TIM_SetCompare3(TIM2, Compare);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, Compare);
}

void PWM_SetCompare2(uint16_t Compare)
{
	//TIM_SetCompare3(TIM2, Compare);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, Compare);
}