#include "main.h"                  // Device header
#include "tim.h"

void Encoder_Init(void)
{
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}

int16_t Encoder_Get(uint8_t n)
{
	int16_t Temp;
	if (n==1) {
		Temp = __HAL_TIM_GET_COUNTER(&htim2);
		__HAL_TIM_SET_COUNTER(&htim2, 0);
		return Temp;
	}
	else if (n==2) {
		Temp = __HAL_TIM_GET_COUNTER(&htim3);
		__HAL_TIM_SET_COUNTER(&htim3, 0);
		return Temp;
	}
	return 0;
}
