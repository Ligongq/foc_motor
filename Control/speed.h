//
// Created by ROG on 2025/7/19.
//

#ifndef __SPEED_H_
#define __SPEED_H_
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdint.h"
#include "start.h"
#include "math.h"
#include "sin.h"
#include "stepmotor_utils.h"
#include "stepmotor.h"
#include "mt6816.h"
#include <stdio.h>
#include <string.h>
#define SPEED_ERR_SUM_MAX 1000.0f
#define SPEED_ERR_SUM_MIN -1000.0f
typedef struct
{
     volatile  float speed_kp;
	volatile  float speed_kd;
     volatile  float speed_ki;
     volatile  int target_speed;
     volatile  int now_speed;
	 volatile int speed_out;

	volatile	uint16_t Mt6816_date_now;
	volatile	float angle_get_now;

} control;
extern  control PID;
void PID_Init(void);
void Speed_PID_Control(float speed_now);
void speed_control_2KHZ(void);
void Speed_Debug_Init(float kp, float ki, float target_speed);
#endif //MOTOR_FOC_SPEED_H
