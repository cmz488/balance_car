/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Encoder.h"
#include "LED.h"
#include "MPU6050.h"
#include "Motor.h"
#include "bsp_dwt.h"
#include "math.h"
#include "PID.h"
#include "key.h"
#include "GraySensor.h"
#include "ChassisCtrl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const float encoder_to_speed = 0.012987012987f;
const float Alpha = 0.01f; //融合滤波系数
const float Gyro_to_real_degree = 0.06103515625f;
const float Degree_to_rad = 0.017453292f;
const float Rad_to_deg = 57.2957795f;
const float speed_filter = 0.01f;
uint8_t tim_flag = 0;
int16_t gray_offset = 0;
uint8_t gray_digital = 0;
uint8_t outline_move_strategy_flag = 0; //0:过十字 1:直行
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*所有用到的参数*/
int16_t AX, AY, AZ, GX, GY, GZ;
uint8_t TimerErrorFlag;
uint16_t TimerCount;

float AngleAcc;
float AngleGyro;
float Angle;

float GY_Bios;
float Yaw;

uint8_t KeyNum, RunFlag = 0;

int16_t LeftPWM, RightPWM;
int16_t AvePWM, DifPWM;

float LeftSpeed, RightSpeed;
float AveSpeed, DifSpeed;

PID_t AnglePID = {
  .Kp = 5.00f,
  .Ki = 0.12f,
  .Kd = 5.0f,

  .OutMax = 100,
  .OutMin = -100,

  .OutOffset = 3,

  .ErrorIntMax = 600,
  .ErrorIntMin = -600,
};

PID_t SpeedPID = {
  .Kp = 0.008f,
  .Ki = 0.00001f,
  .Kd = 0,

  .OutMax = 10,
  .OutMin = -10,

  .ErrorIntMax = 150,
  .ErrorIntMin = -150,
};

PID_t TurnPID = {
  .Kp = 0,
  .Ki = 0.0f,
  .Kd = 0,

  .OutMax = 50,
  .OutMin = -50,

  .ErrorIntMax = 20,
  .ErrorIntMin = -20,
};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  BSP_DWT_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  gray_sensor_init();
  Encoder_Init();
  MPU6050_Init();
  DWT_Delay_ms(500);
  Motor_Init();
  uint8_t key_scan_count = 0;
  uint8_t Count1 = 0;

  //  gray_sensor_init();
  //Serial_receive_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  PID_Init(&AnglePID);
  PID_Init(&SpeedPID);
  PID_Init(&TurnPID);

  HAL_TIM_Base_Start_IT(&htim5);
  while (1) {
    key_scan_count++;
    if (key_scan_count >= 10) {
      KeyNum = key_get_num();
      key_scan_count = 0;
      if (KeyNum == 1) {
        RunFlag = !RunFlag;
      } else if (KeyNum == 2) {
        //button1 点击切换出线策略
        outline_move_strategy_flag = !outline_move_strategy_flag;
      }
    }

    MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);


    if (RunFlag) {
      if (outline_move_strategy_flag == 0) {
        LED_ON(0);
      } else if (outline_move_strategy_flag == 1) {
        LED_OFF(1);
      }
      chassis_ctrl_update();
      AnglePID.Actual = Angle-7.5f;
      PID_Update(&AnglePID);

      AvePWM = -AnglePID.Out;

      LeftPWM = AvePWM + (DifPWM / 2);
      RightPWM = AvePWM - (DifPWM / 2);

      if (LeftPWM > 100) { LeftPWM = 100; } else if (LeftPWM < -100) { LeftPWM = -100; }
      if (RightPWM > 100) { RightPWM = 100; } else if (RightPWM < -100) { RightPWM = -100; }

      Motor_SetPWM(1, LeftPWM);
      Motor_SetPWM(2, RightPWM);
    } else {
      Motor_SetPWM(1, 0);
      Motor_SetPWM(2, 0);
    }

    Count1++;
    if (Count1 >= 5) {
      Count1 = 0;

      LeftSpeed =  (1.0f-speed_filter)*Encoder_Get(1) +speed_filter*LeftSpeed;
      RightSpeed =(1.0f-speed_filter)* Encoder_Get(2) +speed_filter*RightSpeed;

      AveSpeed = (LeftSpeed + RightSpeed) / 2.0f;
      DifSpeed = LeftSpeed - RightSpeed;



      SpeedPID.Actual = AveSpeed;
      PID_Update(&SpeedPID);
      AnglePID.Target = SpeedPID.Out;

      TurnPID.Actual = DifSpeed;
      PID_Update(&TurnPID);
      DifPWM = TurnPID.Out;
    }
    if (tim_flag == 1) {
      tim_flag = 0;
      GY -= 5;
      AngleAcc = -atan2(AX, AZ) / 3.14159f * 180.0f;
      AngleAcc -= 1.43f;
      AngleGyro = Angle + GY * Gyro_to_real_degree * 0.01;
      Angle = Alpha * AngleAcc + (1 - Alpha) * AngleGyro;
      /********************yaw轴计算***********************/
      if (GY < 8) {
        GY_Bios = 0.01f*GY * Gyro_to_real_degree * 0.01+0.99f*GY_Bios;
      }
      Yaw -= GY_Bios;
      Yaw = Yaw + GY * Gyro_to_real_degree * 0.01;

    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  //定时器中断(10ms执行一次)过快会堵塞程序

  if (htim->Instance == TIM5) {
    key_scan();
    tim_flag = 1;
    gray_digital = gray_sensor_get_digital();
    gray_digital = gray_sensor_get_offset();
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
