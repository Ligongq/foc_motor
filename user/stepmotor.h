//
// Created by ROG on 2025/7/10.
//

#ifndef __STEPMOTOR_H_
#define __STEPMOTOR_H_
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdint.h"
#include "start.h"
#include "math.h"
#include "sin.h"
#include "stepmotor_utils.h"

#define AP_H  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define AP_L  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)
#define AM_H  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define AM_L  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define BP_H  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET)
#define BP_L  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET)
#define BM_H  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET)
#define BM_L  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET)
extern uint32_t dac_max;
#define Set_PWM_A(val) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, (val))
#define Set_PWM_B(val) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, (val))
typedef struct{
	uint16_t conver;		//SIN数组指针
	int  sin_data;	//SIN换算数值
	uint16_t dac_reg;		//12位DAC数值
}Coil_Typedef;
extern  Coil_Typedef coil_a;	//电流控制
extern Coil_Typedef coil_b;	//电流控制
void Motor_Sleep(void);
void Motor_Brake(void);
void Motor_Test_Rotate(uint8_t step);
void Motor_MicroStep(uint32_t divide, int32_t elec_ma);
#endif //MOTOR_FOC_MOTOR_H

