//
// Created by ROG on 2025/7/10.
//
#include "start.h"

char send_buff[70];
volatile uint8_t flag_1ms = 0, flag_10ms = 0,flag_100ms=0;
uint16_t divide = 0;
uint32_t micro_idx = 0;
uint8_t rx_byte;
uint8_t systick_20khz_flag=0;
void all(void)
{
	HAL_GPIO_WritePin(MT_CS_GPIO_Port, MT_CS_Pin, GPIO_PIN_SET);

	HAL_UART_Receive_IT(&huart1, &rx_byte, 1);

// 让 SysTick 变成 20 kHz
	Speed_Debug_Init(0,0,100);
	systick_20khz_flag = 1;
	HAL_SYSTICK_Config(SystemCoreClock / 20000);
	for (;;) {
		//Motor_Test_Rotate();
	//	Motor_MicroStep(micro_idx, 200);
		debug_poll();
		if (flag_1ms) {
			flag_1ms = 0;

		}
		if (flag_10ms) {
			flag_10ms = 0;

		}
		if (flag_100ms) {
			flag_100ms = 0;


		}
	}
}
void SysTick_Handler(void)//20KHZ
{
	static uint8_t cnt_1ms , cnt_10ms, cnt_100ms = 0;
	if(systick_20khz_flag) {

		speed_control_2KHZ();
		micro_idx = (micro_idx + PID.speed_out) & 0x03FF;
		Motor_MicroStep(micro_idx, 200);
		if (++cnt_1ms >= 20) { // 20×50us = 1ms
			cnt_1ms = 0;
			flag_1ms = 1;

			if (++cnt_10ms >= 10) { // 10ms
				cnt_10ms = 0;
				flag_10ms = 1;
				uart1_printf("tar=%.1f spd=%.1f\r\n", PID.target_speed, PID.now_speed);
			}
			if (++cnt_100ms >= 100) { // 100ms
				cnt_100ms = 0;
				flag_100ms = 1;

				LED0_TOG;
			}
		}
	}
	else {
		HAL_IncTick();
	}
}

