//
// Created by ROG on 2025/7/10.
//
#include "start.h"
#include <stdint.h>
#include "flash.h"
volatile uint8_t flag_1ms = 0, flag_10ms = 0,flag_100ms=0;
uint16_t divide = 0;
uint32_t micro_idx = 0;
uint8_t rx_byte;
uint8_t systick_20khz_flag=0;
void all(void)
{
	HAL_GPIO_WritePin(MT_CS_GPIO_Port, MT_CS_Pin, GPIO_PIN_SET);
	HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
	if (!Is_LUT_Valid()) {
		uart1_printf("Flash LUT 无效，需重新校准\r\n");
		Encoder_Cali(); // 重新校准并写入
	} else {
		uart1_printf("Flash LUT 有效，加载LUT...\r\n");
		//Print_All_LUT(); // 上电直接全部打印 LUT 数据
	}
	Foc_Init();
	Speed_Debug_Init(10,0,5);
	systick_20khz_flag = 1;
	HAL_SYSTICK_Config(SystemCoreClock / 20000);
	for (;;) {
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
		FOC_Ctrl((int16_t)PID.speed_out, 0, (Sector_tracking() + PID.Lead_angle_Out/4) & (0x03FF) );
		if (++cnt_1ms >= 20) {// 20×50us = 1ms
			cnt_1ms = 0;
			flag_1ms = 1;

			if (++cnt_10ms >= 10) { // 10ms
				cnt_10ms = 0;
				flag_10ms = 1;
				uart1_printf("out=%d sec=%d spd=%.1f\r\n",PID.speed_out,PID.Lead_angle_Out, PID.now_speed);
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

