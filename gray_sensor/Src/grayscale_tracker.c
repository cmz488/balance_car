#include "grayscale_tracker.h"
#include <string.h>

void GRAY_Init(GRAY_HandleTypeDef *hgray);
static uint8_t GRAY_CalcDataLen(uint8_t mode);
static void GRAY_ParsePacket(GRAY_HandleTypeDef *hgray);
static void GRAY_ResetState(GRAY_HandleTypeDef *hgray);

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
void GRAY_ProcessByte(GRAY_HandleTypeDef *hgray, uint8_t data)
{
    switch (hgray->state) {
        case GRAY_STATE_IDLE:
            if (data == GRAY_HEAD1) {
                hgray->state = GRAY_STATE_HEAD1;
            }
            break;

        case GRAY_STATE_HEAD1:
            if (data == GRAY_HEAD2) {
                hgray->state = GRAY_STATE_HEAD2;
                hgray->rx_index = 0;          // 准备从0开始存储
            } else {
                GRAY_ResetState(hgray);
            }
            break;

        case GRAY_STATE_HEAD2:
            // 此时应接收模式位
            hgray->rx_buffer[0] = data;        // 模式位存入缓冲区索引0
            hgray->rx_index = 1;                // 下一个字节将存入索引1
            // 检查模式位高4位是否为0xF（可选，可根据协议决定是否严格校验）
            if ((data & 0xF0) == 0xF0) {
                // 计算数据段长度
                hgray->expected_data_len = GRAY_CalcDataLen(data & 0x0F);
                hgray->data_received = 0;
                if (hgray->expected_data_len == 0) {
                    // 如果没有数据段，则直接等待尾
                    hgray->state = GRAY_STATE_TAIL1;
                } else {
                    hgray->state = GRAY_STATE_DATA;
                }
            } else {
                // 模式位不正确，重新同步
                GRAY_ResetState(hgray);
            }
            break;

        case GRAY_STATE_DATA:
            // 存储数据字节
            if (hgray->rx_index < GRAY_MAX_PACKET_LEN) {
                hgray->rx_buffer[hgray->rx_index++] = data;
            }
            hgray->data_received++;
            if (hgray->data_received >= hgray->expected_data_len) {
                hgray->state = GRAY_STATE_TAIL1;
            }
            break;

        case GRAY_STATE_TAIL1:
            if (data == GRAY_TAIL1) {
                hgray->state = GRAY_STATE_TAIL2;
            } else {
                GRAY_ResetState(hgray);
            }
            break;

        case GRAY_STATE_TAIL2:
            if (data == GRAY_TAIL2) {
                // 完整包接收完成，解析
                GRAY_ParsePacket(hgray);
                GRAY_ResetState(hgray);
            } else {
                GRAY_ResetState(hgray);
            }
            break;

        default:
            GRAY_ResetState(hgray);
            break;
    }
}


/* 初始化句柄 */
void GRAY_Init(GRAY_HandleTypeDef *hgray)
{
    hgray->state = GRAY_STATE_IDLE;
    hgray->rx_index = 0;
    hgray->expected_data_len = 0;
    hgray->data_received = 0;
    hgray->data_ready = 0;
    memset(hgray->rx_buffer, 0, sizeof(hgray->rx_buffer));
    memset(&hgray->data, 0, sizeof(hgray->data));
}

/* 根据模式位计算数据段长度 */
static uint8_t GRAY_CalcDataLen(uint8_t mode)
{
    uint8_t len = 0;
    if (mode & GRAY_MASK_OFFSET)  len += 2;
    if (mode & GRAY_MASK_DIGITAL) len += 1;
    if (mode & GRAY_MASK_ANALOG)  len += 16;
    return len;
}

/* 解析缓冲区中的数据（rx_buffer[0] = mode，后续为数据段） */
static void GRAY_ParsePacket(GRAY_HandleTypeDef *hgray)
{
    uint8_t *buf = hgray->rx_buffer;
    uint8_t mode = buf[0];
    uint8_t *p = buf + 1;   // 指向数据段起始
    hgray->data.mode = mode;

    // 解析偏移量（如果存在）
    if (mode & GRAY_MASK_OFFSET) {
        // 小端：低字节在前
        hgray->data.offset = (uint16_t)p[1] << 8 | p[0];
        p += 2;
    } else {
        hgray->data.offset = 0;  // 清零或保持上次值，这里清零
    }

    // 解析数字量（如果存在）
    if (mode & GRAY_MASK_DIGITAL) {
        hgray->data.digital = *p;
			  for(int i = 0; i < 8; i++)
				{
        // 提取数字量第i位
        hgray->data.digital_signals[i] = (hgray->data.digital >> i) & 0x01;
				}
        p += 1;
    } else {
        hgray->data.digital = 0;
    }

    // 解析8路模拟量（如果存在）
    if (mode & GRAY_MASK_ANALOG) {
        for (int i = 0; i < 8; i++) {
            hgray->data.analog[i] = (uint16_t)p[1] << 8 | p[0];
            p += 2;
        }
    } else {
        // 清零模拟量数组（可选）
        memset(hgray->data.analog, 0, sizeof(hgray->data.analog));
    }

    hgray->data_ready = 1;
}

/* 重置状态机到空闲 */
static void GRAY_ResetState(GRAY_HandleTypeDef *hgray)
{
    hgray->state = GRAY_STATE_IDLE;
    hgray->rx_index = 0;
    hgray->expected_data_len = 0;
    hgray->data_received = 0;
}

/* 检查是否有新数据就绪 */
uint8_t GRAY_IsDataReady(GRAY_HandleTypeDef *hgray)
{
    return hgray->data_ready;
}

/* 获取最新数据，并清除就绪标志 */
GRAY_Data GRAY_GetData(GRAY_HandleTypeDef *hgray)
{
    GRAY_Data data = hgray->data;
    hgray->data_ready = 0;
    return data;
}
