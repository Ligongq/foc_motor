//
// Created by ROG on 2025/7/10.
//
#include "start.h"
char send_buff[70];
volatile uint8_t flag_1ms = 0, flag_10ms = 0,flag_100ms=0;
uint16_t divide = 0;
uint16_t step = 0;
static const uint16_t test_amp[] = {200, 700, 1000};

void all(void)
{
	HAL_GPIO_WritePin(MT_CS_GPIO_Port, MT_CS_Pin, GPIO_PIN_SET);
	HAL_SYSTICK_Config(SystemCoreClock / 20000); // 50us tick
	Speed_Debug_Init(2,0,50);
	for (;;) {
		if (flag_1ms) {
			flag_1ms = 0;
		//	 MT6816_ReadAngleDeg_Alt();
//			sprintf(send_buff,"max=%lu A=%u B=%u\r\n", dac_max, coil_a.dac_reg, coil_b.dac_reg);
//			HAL_UART_Transmit(&huart1,(uint8_t *)send_buff, strlen(send_buff),200);
		}
		if (flag_10ms) {
			flag_10ms = 0;
			sprintf(send_buff,"tar=%.1f spd=%.1f \r\n", PID.target_speed, PID.now_speed);
			HAL_UART_Transmit(&huart1,(uint8_t *)send_buff, strlen(send_buff),200);
		}
		if (flag_100ms) {
			flag_100ms = 0;

			LED0_TOG;
		}
	}
}
void SysTick_Handler(void)//20KHZ
{
	static uint8_t cnt_1ms , cnt_10ms, cnt_100ms = 0;
	static uint32_t micro_idx = 0;
	if (++cnt_1ms >= 20) { // 20×50us = 1ms
		cnt_1ms = 0;
		flag_1ms = 1;
		HAL_IncTick();
		MT6816_ReadAngleDeg_Alt();
		micro_idx = (micro_idx+Speed_PID_Control())&0x03FF;
		Motor_MicroStep(micro_idx, 400);
		if (++cnt_10ms >= 10) { // 10ms
			cnt_10ms = 0;
			flag_10ms = 1;
		}
		if (++cnt_100ms >= 100) { // 100ms
			cnt_100ms = 0;
			flag_100ms = 1;

		}
	}
}

