//
// Created by ROG on 2025/7/16.
//

#ifndef __MT6816_H_
#define __MT6816_H_
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "start.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"
#include "spi.h"
#include "key.h"
#include "stepmotor.h"
#include "sin.h"
#include "speed.h"
#define MT_CS_GPIO_Port GPIOA
#define MT_CS_Pin       GPIO_PIN_15
extern uint16_t mt6816_raw;
extern float mt6816_angle;
uint16_t MT6816_ReadRaw_Alt(void);
void MT6816_ReadAngleDeg_Alt(void);
#endif //MOTOR_FOC_MT6816_H
