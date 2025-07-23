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
#include "speed.h"

struct control
{
     volatile  float speed_kp;
	volatile  float speed_kd;
     volatile  float speed_ki;
	volatile int32_t Increment;
     volatile  float target_speed;
     volatile  float now_speed;
	 volatile int speed_out;

	volatile	uint16_t Mt6816_date_now;
	volatile	float angle_get_now;


	volatile float Leadangle_Kp;
	volatile float Leadangle_Ki;
	volatile float Leadangle_Kd;
	volatile int Lead_angle_Out;
};
extern struct control PID;
void PID_Init(void);
void Speed_PID_Control(float speed_now);
void speed_control_2KHZ(void);
void Angle_PID(void);
void Speed_Debug_Init(float kp, float ki, int target_speed);
#endif //MOTOR_FOC_SPEED_H
