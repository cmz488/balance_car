//
// Created by 34254 on 2026/4/10.
//

#ifndef BALANCE_CAR_GRAYSENSOR_H
#define BALANCE_CAR_GRAYSENSOR_H
#include <stdint.h>
#include "usart.h"
/* 帧定义 */
#define GRAY_HEAD1          0x7E
#define GRAY_HEAD2          0x7E
#define GRAY_TAIL1          0xE7
#define GRAY_TAIL2          0xE7

/* 模式位掩码（低四位） */
#define GRAY_MASK_OFFSET    0x01    // 偏移量存在标志
#define GRAY_MASK_DIGITAL   0x02    // 数字量存在标志
#define GRAY_MASK_ANALOG    0x04    // 模拟量存在标志

/* 最大包长度：2头 + 1模式 + 2偏移 + 1数字 + 16模拟 + 2尾 = 24 */
#define GRAY_MAX_PACKET_LEN 24
/* 解析后的数据 */
typedef struct {
  uint8_t  mode;                   // 原始模式位（高4位固定为0xF）
  int16_t offset;                  // 偏航量
  uint8_t  digital;                 // 数字量
  uint16_t analog[8];               // 8路模拟量
  uint8_t digital_signals[8];       // 数字量单通道数据
} GRAY_Data;
/* 接收状态机状态 */
typedef enum {
  GRAY_STATE_IDLE,                   // 空闲，等待第一个头
  GRAY_STATE_HEAD1,                  // 已收到第一个头，等待第二个头
  GRAY_STATE_HEAD2,                   // 已收到第二个头，准备读模式位
  GRAY_STATE_MODE,                    // 读模式位
  GRAY_STATE_DATA,                     // 读数据段
  GRAY_STATE_TAIL1,                    // 等待第一个尾
  GRAY_STATE_TAIL2                     // 等待第二个尾
} GRAY_State;
typedef struct {
  GRAY_State state;                   // 当前状态
  uint8_t rx_buffer[GRAY_MAX_PACKET_LEN];  // 原始数据缓冲区
  uint8_t rx_index;                    // 缓冲区当前写入索引
  uint8_t expected_data_len;            // 根据模式位计算的数据段总长度（字节）
  uint8_t data_received;                // 数据段已接收字节数
  GRAY_Data data;                       // 最新解析的数据
  uint8_t data_ready;                   // 新数据就绪标志
} GRAY_HandleTypeDef;

#ifdef __cplusplus
extern "C" {
#endif
  void gray_sensor_init();
  int16_t gray_sensor_get_offset();
  uint8_t gray_sensor_get_digital();
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
class GraySensor {
  UART_HandleTypeDef* huart;
  uint8_t rx_row_buffer[GRAY_MAX_PACKET_LEN];
public:
  GRAY_HandleTypeDef gray_handle_;
  GraySensor(UART_HandleTypeDef* huart);
  ~GraySensor() = default;
  void GRAY_Init();
  void GRAY_ProcessByte(uint8_t data);
  uint8_t GRAY_IsDataReady();
  void UART_init();
  static  void UART_call_back(UART_HandleTypeDef *huart, uint16_t Size);
  int16_t get_offset(){return gray_handle_.data.offset;};
  uint8_t get_digital(){return gray_handle_.data.digital;};
private:
  uint8_t GRAY_CalcDataLen(uint8_t mode);
  void GRAY_ParsePacket();
  void GRAY_ResetState();
  GRAY_Data GRAY_GetData();

};
GraySensor* get_gray_sensor_intance();
#endif


#endif //BALANCE_CAR_GRAYSENSOR_H