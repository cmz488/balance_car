//
// Created by 34254 on 2026/4/10.
//

#include "../Inc/GraySensor.h"
#include "cstring"
#include "main.h"
static GraySensor gray_sensor(&huart3);
extern "C" {
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  GraySensor::UART_call_back(huart, Size);
}

void gray_sensor_init() {
  gray_sensor.UART_init();
}

int16_t gray_sensor_get_offset() {
  return gray_sensor.get_offset();
}

uint8_t gray_sensor_get_digital() {
  return gray_sensor.get_digital();
}
}

GraySensor *get_gray_sensor_intance() {
  return &gray_sensor;
}

/**
  * @brief          灰度模块数据解析核心函数（状态机实现）
  * @param[in,out] *hgray: 灰度模块句柄指针，包含状态机变量、解析到的数据等
  * @param[in]      data: 从串口接收到的原始字节
  * @retval         无
  *
  * @note           该函数应在串口接收中断中调用（串口中断每次接收字节数为1）。
  *                 根据模式位自动判断存在哪些数据种类（偏航量、数字量、模拟量）
  *                 偏航量、数字量、模拟量数据储存在句柄结构体变量的GRAY_Data data中
  *                 出现错误时自动复位，等待下一个有效帧头。

	*	@example				void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	*									{
	*											if (huart == &huart3) {
	*													GRAY_ProcessByte(&hgray, rx_byte);
	*													HAL_UART_Receive_IT(&huart3, &rx_byte, 1);
	*											}
	*									}
  */
void GraySensor::GRAY_ProcessByte(uint8_t data) {
  switch (gray_handle_.state) {
    case GRAY_STATE_IDLE:
      if (data == GRAY_HEAD1) {
        gray_handle_.state = GRAY_STATE_HEAD1;
      }
      break;

    case GRAY_STATE_HEAD1:
      if (data == GRAY_HEAD2) {
        gray_handle_.state = GRAY_STATE_HEAD2;
        gray_handle_.rx_index = 0; // 准备从0开始存储
      } else {
        GRAY_ResetState();
      }
      break;

    case GRAY_STATE_HEAD2:
      // 此时应接收模式位
      gray_handle_.rx_buffer[0] = data; // 模式位存入缓冲区索引0
      gray_handle_.rx_index = 1; // 下一个字节将存入索引1
      // 检查模式位高4位是否为0xF（可选，可根据协议决定是否严格校验）
      if ((data & 0xF0) == 0xF0) {
        // 计算数据段长度
        gray_handle_.expected_data_len = GRAY_CalcDataLen(data & 0x0F);
        gray_handle_.data_received = 0;
        if (gray_handle_.expected_data_len == 0) {
          // 如果没有数据段，则直接等待尾
          gray_handle_.state = GRAY_STATE_TAIL1;
        } else {
          gray_handle_.state = GRAY_STATE_DATA;
        }
      } else {
        // 模式位不正确，重新同步
        GRAY_ResetState();
      }
      break;

    case GRAY_STATE_DATA:
      // 存储数据字节
      if (gray_handle_.rx_index < GRAY_MAX_PACKET_LEN) {
        gray_handle_.rx_buffer[gray_handle_.rx_index++] = data;
      }
      gray_handle_.data_received++;
      if (gray_handle_.data_received >= gray_handle_.expected_data_len) {
        gray_handle_.state = GRAY_STATE_TAIL1;
      }
      break;

    case GRAY_STATE_TAIL1:
      if (data == GRAY_TAIL1) {
        gray_handle_.state = GRAY_STATE_TAIL2;
      } else {
        GRAY_ResetState();
      }
      break;

    case GRAY_STATE_TAIL2:
      if (data == GRAY_TAIL2) {
        // 完整包接收完成，解析
        GRAY_ParsePacket();
        GRAY_ResetState();
      } else {
        GRAY_ResetState();
      }
      break;

    default:
      GRAY_ResetState();
      break;
  }
}

GraySensor::GraySensor(UART_HandleTypeDef *huart) {
  this->huart = huart;
  this->GRAY_Init();
}

void GraySensor::UART_init() {
  __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_row_buffer, GRAY_MAX_PACKET_LEN);
}

void GraySensor::UART_call_back(UART_HandleTypeDef *huart, uint16_t Size) {
  if (Size != 0 && huart->Instance == USART3) {
    for (int i = 0; i < Size; i++) {
      gray_sensor.GRAY_ProcessByte(gray_sensor.rx_row_buffer[i]);
    }
  }
  HAL_UARTEx_ReceiveToIdle_DMA(huart, gray_sensor.rx_row_buffer, GRAY_MAX_PACKET_LEN);
}

/* 初始化句柄 */
void GraySensor::GRAY_Init() {
  gray_handle_.state = GRAY_STATE_IDLE;
  gray_handle_.rx_index = 0;
  gray_handle_.expected_data_len = 0;
  gray_handle_.data_received = 0;
  gray_handle_.data_ready = 0;
  memset(gray_handle_.rx_buffer, 0, sizeof(gray_handle_.rx_buffer));
  memset(&gray_handle_.data, 0, sizeof(gray_handle_.data));
}

/* 根据模式位计算数据段长度 */
uint8_t GraySensor::GRAY_CalcDataLen(uint8_t mode) {
  uint8_t len = 0;
  if (mode & GRAY_MASK_OFFSET) len += 2;
  if (mode & GRAY_MASK_DIGITAL) len += 1;
  if (mode & GRAY_MASK_ANALOG) len += 16;
  return len;
}

/* 解析缓冲区中的数据（rx_buffer[0] = mode，后续为数据段） */
void GraySensor::GRAY_ParsePacket() {
  uint8_t *buf = gray_handle_.rx_buffer;
  uint8_t mode = buf[0];
  uint8_t *p = buf + 1; // 指向数据段起始
  gray_handle_.data.mode = mode;

  // 解析偏移量（如果存在）
  if (mode & GRAY_MASK_OFFSET) {
    // 小端：低字节在前
    gray_handle_.data.offset = (uint16_t) p[1] << 8 | p[0];
    p += 2;
  } else {
    gray_handle_.data.offset = 0; // 清零或保持上次值，这里清零
  }

  // 解析数字量（如果存在）
  if (mode & GRAY_MASK_DIGITAL) {
    gray_handle_.data.digital = *p;
    for (int i = 0; i < 8; i++) {
      // 提取数字量第i位
      gray_handle_.data.digital_signals[i] = (gray_handle_.data.digital >> i) & 0x01;
    }
    p += 1;
  } else {
    gray_handle_.data.digital = 0;
  }

  // 解析8路模拟量（如果存在）
  if (mode & GRAY_MASK_ANALOG) {
    for (int i = 0; i < 8; i++) {
      gray_handle_.data.analog[i] = (uint16_t) p[1] << 8 | p[0];
      p += 2;
    }
  } else {
    // 清零模拟量数组（可选）
    memset(gray_handle_.data.analog, 0, sizeof(gray_handle_.data.analog));
  }

  gray_handle_.data_ready = 1;
}

/* 重置状态机到空闲 */
void GraySensor::GRAY_ResetState() {
  gray_handle_.state = GRAY_STATE_IDLE;
  gray_handle_.rx_index = 0;
  gray_handle_.expected_data_len = 0;
  gray_handle_.data_received = 0;
}

/* 检查是否有新数据就绪 */
uint8_t GraySensor::GRAY_IsDataReady() {
  return gray_handle_.data_ready;
}

/* 获取最新数据，并清除就绪标志 */
GRAY_Data GraySensor::GRAY_GetData() {
  GRAY_Data data = gray_handle_.data;
  gray_handle_.data_ready = 0;
  return data;
}
