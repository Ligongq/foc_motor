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
#define ENCODER_MAX_VALUE 16384
#define SECTOR_TOTAL_COUNT 200
#define SECTOR_DIVISION_RESULT 82
typedef struct{
	volatile uint8_t	sector;
	volatile int	scope;
	volatile int16_t angle;
	volatile int lead_angle;
	volatile float speed_error;
	uint8_t offset;
}FOC_parameter_t;
extern FOC_parameter_t foc;
void FOC_InPark_Out(uint32_t _directionIncount, int32_t Iq);
void Foc_Init(void);
uint16_t Sector_tracking(void);
void FOC_Ctrl(int16_t Iq, int16_t Id,int x);
#endif //MOTOR_FOC_FOC_H
