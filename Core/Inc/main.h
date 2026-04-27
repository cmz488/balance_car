/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BOTTON1_Pin GPIO_PIN_4
#define BOTTON1_GPIO_Port GPIOE
#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOF
#define BOTTON0_Pin GPIO_PIN_1
#define BOTTON0_GPIO_Port GPIOC
#define A_MOTOR_ENCODER_1_Pin GPIO_PIN_1
#define A_MOTOR_ENCODER_1_GPIO_Port GPIOA
#define A_MOTOR_ENCODER_2_Pin GPIO_PIN_5
#define A_MOTOR_ENCODER_2_GPIO_Port GPIOA
#define B_MOTOR_ENCODER_1_Pin GPIO_PIN_6
#define B_MOTOR_ENCODER_1_GPIO_Port GPIOA
#define B_MOTOR_ENCODER_2_Pin GPIO_PIN_7
#define B_MOTOR_ENCODER_2_GPIO_Port GPIOA
#define AIN_1_Pin GPIO_PIN_0
#define AIN_1_GPIO_Port GPIOB
#define PWM_A_Pin GPIO_PIN_9
#define PWM_A_GPIO_Port GPIOE
#define PWM_B_Pin GPIO_PIN_11
#define PWM_B_GPIO_Port GPIOE
#define AIN_2_Pin GPIO_PIN_13
#define AIN_2_GPIO_Port GPIOB
#define BUZZ_Pin GPIO_PIN_12
#define BUZZ_GPIO_Port GPIOD
#define BIN_1_Pin GPIO_PIN_3
#define BIN_1_GPIO_Port GPIOD
#define BIN_2_Pin GPIO_PIN_5
#define BIN_2_GPIO_Port GPIOD
#define IIC_SLK_Pin GPIO_PIN_11
#define IIC_SLK_GPIO_Port GPIOG
#define IIC_SDA_Pin GPIO_PIN_13
#define IIC_SDA_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
