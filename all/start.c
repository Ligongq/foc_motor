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
	HAL_SYSTICK_Config(SystemCoreClock / 20000); // 50us tick
	HAL_GPIO_WritePin(MT_CS_GPIO_Port, MT_CS_Pin, GPIO_PIN_SET);

	for (;;) {
//		for (unsigned i = 0; i < sizeof(test_amp)/sizeof(test_amp[0]); ++i) {
//			uint16_t amp = test_amp[i];
//			Motor_MicroStep(0, amp);                        // 锁 A 相峰值
//			sprintf(send_buff,"amp=%u mA  dac_max=%u duty=%u/1023 VREF≈%.2f V\r\n",amp, dac_max, dac_max, 3.3f * dac_max / 1023.0f);
//			HAL_UART_Transmit(&huart1,(uint8_t *)send_buff, strlen(send_buff),200);
//			HAL_Delay(30000);                                // 3 s 测量窗口
//		}
//		Motor_Sleep();                                     // 结束后关桥
//		while (1);

		if (flag_1ms) {
			flag_1ms = 0;

			divide = (divide + 8) & 0x03FF;
			Motor_MicroStep(divide, 300);
//			sprintf(send_buff,"max=%lu A=%u B=%u\r\n", dac_max, coil_a.dac_reg, coil_b.dac_reg);
//			HAL_UART_Transmit(&huart1,(uint8_t *)send_buff, strlen(send_buff),200);

		}
		if (flag_10ms) {
			flag_10ms = 0;
			mt6816_raw = MT6816_ReadRaw_Alt();
			mt6816_angle = MT6816_ReadAngleDeg_Alt();
			sprintf(send_buff,"raw=%d ang=%.1f\r\n", mt6816_raw, mt6816_angle);
			HAL_UART_Transmit(&huart1,(uint8_t *)send_buff, strlen(send_buff),200);
		}
		if (flag_100ms) {
			flag_100ms = 0;
		//	LED0_TOG;
		}
	}
}
void SysTick_Handler(void)
{
	HAL_IncTick();
	static uint8_t cnt_1ms = 0;
	static uint8_t cnt_10ms = 0;
	static uint8_t cnt_100ms = 0;

	if (++cnt_1ms >= 20) { // 20×50us = 1ms
		cnt_1ms = 0;
		flag_1ms = 1;
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

