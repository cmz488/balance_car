//
// Created by 34254 on 2025/12/20.
//

#ifndef F407_LEARN_BSP_CAN_H
#define F407_LEARN_BSP_CAN_H

#pragma once

#ifdef  __cplusplus
extern "C"{
#endif

#include "main.h"
#include "can.h"
#include "dbus.h"

  typedef struct  {
int16_t rotor_angle;
int16_t rotor_speed;
int16_t torque_current;
int8_t temp;
} DJI_motor_info;

extern DBUS_t dbus;
extern uint16_t id;
extern DJI_motor_info motor1;
extern DJI_motor_info motor2;
extern uint8_t is_rx[4];

HAL_StatusTypeDef CAN_MOTOR_CMD(int16_t motor1,int16_t motor2,int16_t  motor3,int16_t motor4);
void BSP_CAN_Init(void);

#ifdef  __cplusplus
}
#endif

#ifdef __cplusplus

class BspCan{
private:
public:
    void bsp_can_init(void);
    void bsp_can_filterconfig(void);
    HAL_StatusTypeDef can_motor_cmd(int16_t motor1,int16_t motor2,int16_t motor3,int16_t motor4);
};

#endif



#endif //F407_LEARN_BSP_CAN_H
