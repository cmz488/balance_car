//
// Created by 34254 on 2025/12/20.
//
#include "bsp_can.h"

extern "C"
{
  uint16_t id, preid;
  BspCan bsp_can;
  DJI_motor_info motor1, motor2, motor3, motor4;
  HAL_StatusTypeDef CAN_MOTOR_CMD(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
  {
    return bsp_can.can_motor_cmd(motor1, motor2, motor3, motor4);
  }
  void BSP_CAN_Init(void)
  {

    bsp_can.bsp_can_filterconfig();

    bsp_can.bsp_can_init();
  }
}

HAL_StatusTypeDef BspCan::can_motor_cmd(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
  CAN_TxHeaderTypeDef Txheader;
  uint8_t TxData[8];
  uint32_t TxMailbox;
  TxData[0] = motor1 >> 8;
  TxData[1] = motor1;
  TxData[2] = motor2 >> 8;
  TxData[3] = motor2;
  TxData[4] = motor3 >> 8;
  TxData[5] = motor3;
  TxData[6] = motor4 >> 8;
  TxData[7] = motor4;
  Txheader.DLC = 8;
  Txheader.StdId = 0x1ff;
  Txheader.IDE = CAN_ID_STD;
  Txheader.RTR = CAN_RTR_DATA;
  return HAL_CAN_AddTxMessage(&hcan1, &Txheader, TxData, &TxMailbox);
}
void BspCan::bsp_can_init(void)
{
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }
}
void BspCan::bsp_can_filterconfig(void)
{
  CAN_FilterTypeDef filter;

  /** ------------ CAN1：过滤器 0-13 全部接收 ------------ */
  filter.FilterActivation = ENABLE;
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;

  filter.FilterIdHigh = 0x0000;
  filter.FilterIdLow = 0x0000;
  filter.FilterMaskIdHigh = 0x0000;
  filter.FilterMaskIdLow = 0x0000;

  filter.FilterBank = 0;            // CAN1 的第一个过滤器组
  filter.SlaveStartFilterBank = 14; // 14 之后给 CAN2 用
  HAL_CAN_ConfigFilter(&hcan1, &filter);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef Rxheader;
  uint8_t RxData[8];
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &Rxheader, RxData) == HAL_OK)
  {
    if (hcan->Instance == CAN1)
    {
      switch (Rxheader.StdId)
      {
      case 0x201:
        is_rx[0] = 1;
        break;
      case 0x202:
        is_rx[1] = 1;
        break;
      case 0x203:
        is_rx[2] = 1;
        break;
      case 0x204:
        is_rx[3] = 1;
        break;
      default:
        break;
      }
      switch (Rxheader.StdId)
      {
      case 0x201:
        motor1.rotor_angle = RxData[0] << 8 | RxData[1];
        motor1.rotor_speed = RxData[2] << 8 | RxData[3];
        motor1.torque_current = RxData[4] << 8 | RxData[5];
        motor1.temp = RxData[6];
        break;
      case 0x202:
        motor2.rotor_angle = RxData[0] << 8 | RxData[1];
        motor2.rotor_speed = RxData[2] << 8 | RxData[3];
        motor2.torque_current = RxData[4] << 8 | RxData[5];
        motor2.temp = RxData[6];
        break;
      case 0x203:
        motor3.rotor_angle = RxData[0] << 8 | RxData[1];
        motor3.rotor_speed = RxData[2] << 8 | RxData[3];
        motor3.torque_current = RxData[4] << 8 | RxData[5];
        motor3.temp = RxData[6];
        break;
      case 0x204:
        motor4.rotor_angle = RxData[0] << 8 | RxData[1];
        motor4.rotor_speed = RxData[2] << 8 | RxData[3];
        motor4.torque_current = RxData[4] << 8 | RxData[5];
        motor4.temp = RxData[6];
        break;
      case 0x205:
        motor1.rotor_angle = RxData[0] << 8 | RxData[1];
        motor1.rotor_speed = RxData[2] << 8 | RxData[3];
        motor1.torque_current = RxData[4] << 8 | RxData[5];
        motor1.temp = RxData[6];
        break;
      case 0x206:
        motor2.rotor_angle = RxData[0] << 8 | RxData[1];
        motor2.rotor_speed = RxData[2] << 8 | RxData[3];
        motor2.torque_current = RxData[4] << 8 | RxData[5];
        motor2.temp = RxData[6];
        break;
      }
    }
  }
}
