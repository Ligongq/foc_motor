#include "mt6816.h"
#include "speed.h"
uint16_t mt6816_raw=0;
float mt6816_angle=0;
extern SPI_HandleTypeDef hspi1;
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
	PID.angle_get_now = (float)raw * (360.0f / 16384.0f);
}
