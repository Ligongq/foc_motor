//
// Created by ROG on 2025/7/22.
//

#ifndef __FOC_H_
#define __FOC_H_
#include "main.h"
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
#include "speed.h"
#define ENCODER_MAX_VALUE 16384
#define SECTOR_TOTAL_COUNT 200

 struct FOC_parameter_t{
	volatile uint8_t	sector;
	volatile int	scope;
	volatile int16_t angle;
	volatile int lead_angle;
	volatile float speed_error;
	 volatile uint8_t offset;
};
extern struct FOC_parameter_t foc;
void FOC_InPark_Out(uint32_t _directionIncount, int32_t Iq);
void Foc_Init(void);
void Sector_tracking(void);
void FOC_Ctrl(int16_t Iq, int16_t Id);
#endif //MOTOR_FOC_FOC_H
