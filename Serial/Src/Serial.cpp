//
// Created by 34254 on 2026/1/12.
//
/*
 * 开启dma接受循环模式
 *
 *
 *
 *  环形缓冲区处理数据
 *
 *  上位机发送数据格式：{
 *    00 帧头
 *    0x p = 1,i = 2,d = 3 参数类型
 *    0x x代表哪一个电机，就是更改第几个pid参数
 *    float的数据}
 */

#include "../Inc/Serial.h"
#include "usart.h"
#include "cstring"
#include "cstdio"
UART_HandleTypeDef *Serial::huart_ = &huart1; // 默认使用 huart1，可以通过 huart_setter 修改
Serial serial;
extern "C" {
  void relocate(float *a1,float *a2,float *a3,float *a4,float *a5,float *a6) {
    serial.relocate({a1,a2,a3},{a4,a5,a6},{});
  }
  void Serial_receive_init() {
    serial.receive_init();
  }
  void pre_write_c(int count,...) {

    float buf[11]; // 10 个参数 + frame_tail
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count+1; i++) {
      buf[i] = va_arg(args, float); // float 会被提升为 double
    }
    va_end(args);
    buf[count+1] = (0x00 << 24) | 0x00 << 16 | 0x80 << 8 | 0x7f; // 帧尾
    HAL_UART_Transmit(&huart1,
                      (uint8_t*)(buf),
                      (count + 2) * sizeof(float),
                      1000);
  }
}

Serial::Serial() : tail(0), head(0), last_cndtr(BUFFER_MAX_SIZE) {
  frame_tail.b[0] = 0x00;
  frame_tail.b[1] = 0x00;
  frame_tail.b[2] = 0x80;
  frame_tail.b[3] = 0x7F;
  std::memset(buffer, 0, BUFFER_MAX_SIZE);
}

void Serial::relocate(std::list<float *> p, std::list<float *> i, std::list<float *> d) {
  for (auto j: p) {
    this->p_ptr.push_back(j);
  }
  for (auto j: i) {
    this->i_ptr.push_back(j);
  }
  for (auto j: d) {
    this->d_ptr.push_back(j);
  }
}

void Serial::receive_init() {
  __HAL_UART_DISABLE_IT(huart_, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(huart_, UART_IT_IDLE);
  HAL_UARTEx_ReceiveToIdle_DMA(huart_, buffer, BUFFER_MAX_SIZE);
}
extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart == serial.huart_) {
    __HAL_DMA_DISABLE(serial.huart_->hdmarx);
    serial.huart_->hdmarx->Instance->NDTR = BUFFER_MAX_SIZE; // 重置NDTR
    __HAL_DMA_ENABLE(serial.huart_->hdmarx);
    serial.process_buffer_CallBack(serial.buffer, Size);
    HAL_UARTEx_ReceiveToIdle_DMA(serial.huart_, serial.buffer, BUFFER_MAX_SIZE);
  }
}

void  Serial::process_buffer_CallBack(uint8_t *buffer, uint16_t length) const {
  if (length < 7) return; // 最短合法帧：00 + type(1B) + idx(1B) + float(4B) + 可能的尾巴
  uint8_t *end_ptr = buffer + length;
  for (uint8_t *ptr = buffer; ptr + 6 < end_ptr; ptr++) {
    if (ptr[0] != 0x00) {
      return;
    }
    if (ptr[0]==0x00){
      switch (ptr[1]) {
        case 0x01: // p
          memcpy(p_ptr[ptr[2] - 1], ptr + 3, 4);
          break;
        case 0x02: // i
          memcpy(i_ptr[ptr[2] - 1], ptr + 3, 4);
          break;
        case 0x03: // d
          memcpy(d_ptr[ptr[2] - 1], ptr + 3, 4);
          break;
        default:
          break;
      }
    }
  }
}


// void Serial::UART_IDLE_Callback() {
//   // 在外部调用__HAL_UART_CLEAR_IDLEFLAG(huart_);
//   process_data();
// }
//
// void Serial::process_data() {
//   // 计算当前DMA写入位置（tail）
//   uint32_t remain = __HAL_DMA_GET_COUNTER(huart_->hdmarx);
//   tail += (last_cndtr - remain); // tail前进相应位置
//   tail %= BUFFER_MAX_SIZE; // 环形缓冲区取模,超出数据缓冲区就环回到开头
//
//   // 计算接收到的数据长度
//   uint16_t received_len;
//   if (tail >= head) {
//     // 无绕回：head → tail
//     received_len = tail - head;
//   } else {
//     // 有绕回：从head到缓冲区末尾 + 从缓冲区开始到tail
//     received_len = (BUFFER_MAX_SIZE - head) + tail;
//   }
//
//   // 处理环形绕回情况
//   if (tail > head) {
//     // 情况1：数据连续，无绕回
//     Process_buffer_CallBack(&buffer[head], received_len);
//   } else {
//     // 情况2：数据绕回，分两段处理
//     // 第一段：从head到缓冲区末尾
//     uint16_t first_part_len = BUFFER_MAX_SIZE - head;
//     Process_buffer_CallBack(&buffer[head], first_part_len);
//
//     // 第二段：从缓冲区开始到tail
//     if (tail > 0) {
//       Process_buffer_CallBack(&buffer[0], tail);
//     }
//
//     // 更新head位置
//     head = tail;
//     last_cndtr = remain;
//   }
// }
//
// void Serial::Process_buffer_CallBack(uint8_t *buffer, uint16_t length) {
//   if (length < 7) return; // 最短合法帧：00 + type(1B) + idx(1B) + float(4B) + 可能的尾巴
//
//   uint8_t *end_ptr = buffer + length;
//
//   for (uint8_t *ptr = buffer; ptr + 6 < end_ptr; ptr++) {
//     // 至少保证能读到float
//     if (ptr[0] != 0x00) continue;
//
//     uint8_t type = ptr[1];
//     uint8_t motor_idx = ptr[2]; // 1-based
//
//     if (motor_idx == 0) continue; // 非法编号
//
//     size_t idx = motor_idx - 1; // 转0-based
//
//     if (type == 1) {
//       if (idx < p_ptr.size()) {
//         memcpy(p_ptr[idx], ptr + 3, 4);
//       }
//     } else if (type == 2) {
//       if (idx < i_ptr.size()) {
//         memcpy(i_ptr[idx], ptr + 3, 4);
//       }
//     } else if (type == 3) {
//       if (idx < d_ptr.size()) {
//         memcpy(d_ptr[idx], ptr + 3, 4);
//       }
//     }
//   }
// }
//
//
// std::vector<float> Serial::insert_data_to_vec(std::list<float> &values) {
//   std::vector<float> rx_buffer;
//   for (float v: values) {
//     rx_buffer.push_back(v);
//   }
//   rx_buffer.push_back(frame_tail.f);
//   return rx_buffer;
// }
