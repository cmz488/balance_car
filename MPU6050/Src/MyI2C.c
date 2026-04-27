#include "main.h"                  // Device header
#include "bsp_dwt.h"
#define IIC_SLK_Pin GPIO_PIN_11
#define IIC_SDA_Pin GPIO_PIN_13
void MyI2C_W_SCL(uint8_t BitValue)
{
	HAL_GPIO_WritePin(GPIOG, IIC_SLK_Pin, BitValue);
	DWT_Delay_us(10);
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	HAL_GPIO_WritePin(GPIOG, IIC_SDA_Pin, BitValue);
	DWT_Delay_us(10);
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = HAL_GPIO_ReadPin(GPIOG, IIC_SDA_Pin);
	DWT_Delay_us(10);
	return BitValue;
}

void MyI2C_Init(void)
{
	//我在使用CubeMX生成代码时，已经将PB10和PB11配置为开漏输出，并且默认输出高电平，所以这里就不再重复初始化了
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//
	// GPIO_InitTypeDef GPIO_InitStructure;
	// GPIO_InitStructure.Mode = GPIO_Mode_Out_OD;
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	//
	// GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
}

void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(!!(Byte & (0x80 >> i)));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA()){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}
