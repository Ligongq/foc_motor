#include "mt6816.h"
// 配置参数
#define STEP_NUM            200
#define SAMPLE_PER_STEP     16
#define MICRO_PER_FULLSTEP  (51200/STEP_NUM)
#define MICRO_PER_REV       51200
uint16_t SampleDataAverage[201];
uint16_t FlashLUT[200];

static inline void MT_CS_L(void) {	HAL_GPIO_WritePin(MT_CS_GPIO_Port, MT_CS_Pin, GPIO_PIN_RESET);}
static inline void MT_CS_H(void) {	HAL_GPIO_WritePin(MT_CS_GPIO_Port, MT_CS_Pin, GPIO_PIN_SET);}
uint16_t MT6816_ReadRaw_Alt(void)
{
	uint16_t cmd, res;
	// 读低8位
	cmd = (0x80 | 0x03) << 8;
	MT_CS_L();
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&cmd, (uint8_t*)&res, 1, HAL_MAX_DELAY);
	MT_CS_H();
	uint16_t data = (res & 0xFF) << 8;
	// 读高8位
	cmd = (0x80 | 0x04) << 8;
	MT_CS_L();
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&cmd, (uint8_t*)&res, 1, HAL_MAX_DELAY);
	MT_CS_H();
	data |= (res & 0xFF);
	// 右移2位（去掉状态），只保留14位
	return (data >> 2) & 0x3FFF;
}
void MT6816_ReadAngleDeg_Alt(void)
{
	uint16_t raw = MT6816_ReadRaw_Alt();
	if (raw == 0xFFFF) {
		/* SPI read failed, keep previous value */
		PID.angle_get_now = -1.0f;
		return;
	}
	PID.Mt6816_date_now = raw;
	//PID.angle_get_now = (float)raw * (360.0f / 16384.0f);
}
void Encoder_Cali(void)
{
	uint16_t i = 0;
	Set_PWM_B(200);
	Set_PWM_A(200);
	HAL_Delay(50);
	MT6816_ReadAngleDeg_Alt();
	for (uint16_t i = 0; i < 200; i++) {
		Motor_Test_Rotate(i%4);
		HAL_Delay(100);
		MT6816_ReadAngleDeg_Alt();
		SampleDataAverage[i] = PID.Mt6816_date_now;
		uart1_printf("%d\r\n",SampleDataAverage[i]);
	}
	Set_PWM_B(0);
	Set_PWM_A(0);
	Write_CaliData_Array(SampleDataAverage, 200);
	Flash_Write_Zero(SampleDataAverage[199]);
	uart1_printf("校准完成并写入Flash\r\n");
}
void Print_All_LUT(void)
{
	uart1_printf("Flash LUT 数据:\r\n");
	for (uint16_t i = 0; i < 200; i++) {
		FlashLUT[i] = Flash_Read_SectorAngle(i);
		uart1_printf("LUT[%d]=%d\r\n", i, FlashLUT[i]);
		HAL_Delay(10);
	}
	uint16_t zero = Flash_Read_Original_Point();
	uart1_printf("原点值：%d\r\n", zero);
}


static void drive_to_micro(uint32_t micro)
{
	micro &= (MICRO_PER_REV-1);
	uint16_t idx = (uint32_t)micro * 1024 / MICRO_PER_REV;      /* 0–1023 */
	Set_PWM_A(sin_pi_m2[idx]);
	Set_PWM_B(sin_pi_m2[idx+256]);
}

void Encoder_Cali_Run(void) {
	static uint16_t tmp_lut[1024];
	uint16_t fwd[STEP_NUM] = {0}, rev[STEP_NUM] = {0};
	uint16_t avg[STEP_NUM] = {0};
	uint32_t micro = 0;
	// 0. 安全停
	Set_PWM_B(0);
	Set_PWM_B(0);
	HAL_Delay(50);

	uart1_printf("[CALI] 正向采样...\r\n");
	// 1. 正向采样
	for (uint16_t s = 0; s < STEP_NUM; ++s) {
		drive_to_micro(micro);
		HAL_Delay(10);
		uint32_t sum = 0;
		MT6816_ReadAngleDeg_Alt();
		for (int k = 0; k < SAMPLE_PER_STEP; k++) sum += PID.Mt6816_date_now;
		fwd[s] = (sum / SAMPLE_PER_STEP) & 0x3FFF;
		micro += MICRO_PER_FULLSTEP;
	}

	uart1_printf("[CALI] 反向采样...\r\n");
	// 2. 反向采样
	micro = MICRO_PER_REV;
	for (int s = STEP_NUM - 1; s >= 0; --s) {
		micro -= MICRO_PER_FULLSTEP;
		drive_to_micro(micro);
		HAL_Delay(10);
		uint32_t sum = 0;
		MT6816_ReadAngleDeg_Alt();
		for (int k = 0; k < SAMPLE_PER_STEP; k++) sum += PID.Mt6816_date_now;
		rev[s] = (sum / SAMPLE_PER_STEP) & 0x3FFF;
	}

	// 3. 双向均值
	for (uint16_t i = 0; i < STEP_NUM; i++)
		avg[i] = (uint16_t) ((fwd[i] + rev[i]) >> 1);

	// 4. 找跨零点
	int cross = -1;
	for (int i = 0; i < STEP_NUM - 1; i++)
		if (avg[i] > avg[i + 1]) {
			cross = i;
			break;
		}
	if (cross < 0) {
		uart1_printf("Cross not found!\r\n");
		return;
	}

	// 5. 生成16384点LUT
	uint16_t ptr = 0;
	for (int seg = 0; seg < STEP_NUM; ++seg) {
		int i = (cross + seg) % STEP_NUM;
		int j = (i + 1) % STEP_NUM;
		int16_t da = (int16_t) (avg[j] - avg[i]);
		if (da < 0) da += 16384;
		for (int k = 0; k < da; k++) {
			uint32_t micro_u = seg * MICRO_PER_FULLSTEP + (uint32_t) k * MICRO_PER_FULLSTEP / da;
			tmp_lut[ptr++] = micro_u;
//			if (ptr >= LUT_SIZE) break;
//		}
//		if (ptr >= LUT_SIZE) break;
//	}
//	if (ptr != LUT_SIZE) {
//		uart1_printf("LUT fill error %u!\r\n", ptr);
//		return;
//	}

			// 6. 存储到 Flash
//	uart1_printf("[CALI] 写入 Flash...\r\n");
//	__disable_irq();
//	int ok = cali_flash_save(tmp_lut);
//	__enable_irq();
//	uart1_printf(ok ? "[CALI] Flash OK\r\n" : "[CALI] Flash FAIL!\r\n");

			// 7. 复位
			drive_to_micro(0);
			Motor_Sleep();
		}
	}
}
