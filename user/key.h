//
// Created by ROG on 2025/7/10.
//

#ifndef __KEY_H_
#define __KEY_H_
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdint.h"
#include "start.h"

#define LED0_TOG HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
#define LED1_TOG HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);

typedef enum {
	BUTTON_IDLE = 0,
	BUTTON_SHORT,
	BUTTON_LONG
} ButtonEvent_t;

void Button_Scan_10ms(void);     // 每10ms调用一次
//ButtonEvent_t Button1_Read(void);
//ButtonEvent_t Button2_Read(void);
#endif
