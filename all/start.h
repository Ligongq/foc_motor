//
// Created by ROG on 2025/7/10.
//
#ifndef __START_H_
#define __START_H_

#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"
#include "key.h"
#include "stepmotor.h"
#include "mt6816.h"
#include "speed.h"
#include "debug.h"
#include "stepmotor_utils.h"
#include "dma.h"
void all(void);
extern char send_buff[70];
#endif //MOTOR_FOC_START_H
