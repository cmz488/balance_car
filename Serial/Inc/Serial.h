//
// Created by 34254 on 2026/1/12.
//

#ifndef VOFA_UART_SERIAL_H
#define VOFA_UART_SERIAL_H
#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "stdarg.h"
#include "stdint.h"
#include "main.h"
  void pre_write_c(int count,...);

#define PP_ARG_N(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N
#define COUNT_ARGS(...) PP_ARG_N(__VA_ARGS__,9,8,7,6,5,4,3,2,1,0)
#define write_c(...) pre_write_c(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

  void Serial_UART_IDLE_Callback();
  void relocate(float *a1,float *a2,float *a3,float *a4,float *a5,float *a6);
  void Serial_receive_init();
#ifdef __cplusplus
}
#endif

#include "main.h"
#define BUFFER_MAX_SIZE 512
#define CMD_MAX_SIZE 48 // 单条命令最大长度 16 + 32float

#ifdef __cplusplus
#include <array>
#include <list>
#include <vector>

typedef union {
  float f;
  uint8_t b[4];
} send_data;

class Serial {
  std::vector<float> data_; // 发送数据缓存区
  send_data frame_tail; // 发送数据帧尾

  uint8_t buffer[BUFFER_MAX_SIZE]; // 环形缓冲区
  uint32_t tail; // 尾指针
  uint32_t head; // 头指针
  uint32_t last_cndtr; // 上次DMA剩余数据计数器

  std::vector<float *> p_ptr; // PID参数指针数组
  std::vector<float *> i_ptr;
  std::vector<float *> d_ptr;

  static UART_HandleTypeDef *huart_; // 串口句柄指针
public:
  Serial();

  static void set_uarthandle(UART_HandleTypeDef *huart) { huart_ = huart; }; // 设置串口句柄
  void relocate(std::list<float *> p, std::list<float *> i, std::list<float *> d
  ); // 重定位PID参数指针

  void receive_init(); // 开启空闲中断接收


  template<typename... Args>
  void write(Args... args) {
    std::array<float, sizeof...(Args)+1> data{{static_cast<float>(args)...,frame_tail.f}};
    HAL_UART_Transmit(huart_,
                      reinterpret_cast<uint8_t *>(data.data()),
                      data.size() * sizeof(float),
                      1000);
  }

  friend void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

  void process_buffer_CallBack(uint8_t *buffer, uint16_t length) const;

  // void UART_IDLE_Callback(); // 空闲中断回调函数
  //
  // void process_data(); // 处理接收到的数据
  //
  // void Process_buffer_CallBack(uint8_t *buffer, uint16_t length);
  //
  // std::vector<float> insert_data_to_vec(std::list<float> &values);
};
#endif

#endif // VOFA_UART_SERIAL_H
