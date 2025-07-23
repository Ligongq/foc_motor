//
// Created by ROG on 2025/7/22.
//

#include "foc.h"
#include "stdlib.h"
int16_t Scope[200];
#define PWM_MAX     1023  // PWM周期
#define RATED_I_MA   1600     /* 3.3 V / (10×0.30) ≈ 1100 mA */
struct FOC_parameter_t foc;
void Foc_Init(void)
{
	foc.angle  = 0;
	foc.scope  = 0;
	foc.sector = 0;
//	for (uint8_t i = 0; i < SECTOR_TOTAL_COUNT; i++) {
//		Scope[i] = Flash_Read_SectorAngle(i + 1) - Flash_Read_SectorAngle(i);
//		if (Scope[i] > SECTOR_TOTAL_COUNT) {
//			Scope[i] += ENCODER_MAX_VALUE;
//			foc.offset = i;
//		}
//	}
	for (uint8_t i = 0; i < SECTOR_TOTAL_COUNT; i++) {
		uint8_t next = (i + 1) % SECTOR_TOTAL_COUNT;
		int16_t diff = (int16_t)Flash_Read_SectorAngle(next) - (int16_t)Flash_Read_SectorAngle(i);
		if (diff < 0) {
			diff += ENCODER_MAX_VALUE;
			foc.offset = i;
		}
		Scope[i] = (uint16_t)diff;
	}
}

uint16_t Sector_tracking(void)
{
//	uint16_t temp = (PID.Mt6816_date_now / SECTOR_DIVISION_RESULT + foc.offset) % SECTOR_TOTAL_COUNT; // 获取扇区大致位置
//	for (int i = temp - 1; i <= temp + 1; i++)                                                       // 在这个粗略计算的扇区的上一个和下一个比较一下看当前的角度是不是在这两个之间从而确定扇区
//	{
//		if (Flash_Read_SectorAngle(i + 1) > Flash_Read_SectorAngle(i)) {
//			if (PID.Mt6816_date_now >= Flash_Read_SectorAngle(i) && PID.Mt6816_date_now <= Flash_Read_SectorAngle(i + 1)) // 在这两个角度数据之间 那么就是在这个扇区
//			{
//				foc.sector = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT; // 让扇区序号的大小在200以内
//				break;                                                      // 判断完毕，退出FOR
//			}
//			// 以下处理特殊情况，也就是编码器跨越零点的时候
//		} else if (Flash_Read_SectorAngle(i + 1) < Flash_Read_SectorAngle(i)) // 跨越零点的时候
//		{
//			if (PID.Mt6816_date_now >= Flash_Read_SectorAngle(i) && PID.Mt6816_date_now <= (Flash_Read_SectorAngle(i + 1) + ENCODER_MAX_VALUE)) {
//				foc.sector = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT; // 让扇区序号的大小在200以内
//				break;                                                      // 判断完毕，退出循环
//			}
//		}
//	}
//	//-----------------------------------------------------------准确找到当前角度在哪个扇区------------------------------------------------------
//	//-----------------------------------------------------------计算出当前的实际角度对应的电角度-------------------------------------------------
//	foc.scope = Scope[foc.sector];
//	if (Flash_Read_SectorAngle(foc.sector + 1) > Flash_Read_SectorAngle(foc.sector)) {
//		foc.angle = (PID.Mt6816_date_now - Flash_Read_SectorAngle(foc.sector)) * 256 / foc.scope + (foc.sector % 4) * 256;
//	} else { // 处理零点跨越的情况
//		if (PID.Mt6816_date_now > 16200) {
//			foc.angle = (PID.Mt6816_date_now - Flash_Read_SectorAngle(foc.sector)) * 256 / foc.scope + (foc.sector % 4) * 256;
//		} else {
//			foc.angle = (16384 - Flash_Read_SectorAngle(foc.sector) + PID.Mt6816_date_now) * 256 / foc.scope + (foc.sector % 4) * 256;
//		}
//	}
//	foc.angle=foc.angle & (0x03FF);
//	return foc.angle;
	// 1. 粗定位，估算大致落在哪个扇区
	uint16_t temp = (PID.Mt6816_date_now / SECTOR_DIVISION_RESULT + foc.offset) % SECTOR_TOTAL_COUNT;

	// 2. 在前后各1个扇区内“夹逼”查找真正所在分段
	for (int i = temp - 1; i <= temp + 1; i++) {
		uint16_t idx1 = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT;
		uint16_t idx2 = (idx1 + 1) % SECTOR_TOTAL_COUNT;
		uint16_t angle1 = Flash_Read_SectorAngle(idx1);
		uint16_t angle2 = Flash_Read_SectorAngle(idx2);

		if (angle2 > angle1) { // 普通区间
			if (PID.Mt6816_date_now >= angle1 && PID.Mt6816_date_now < angle2) {
				foc.sector = idx1;
				break;
			}
		} else { // 跨零点区间
			if (PID.Mt6816_date_now >= angle1 || PID.Mt6816_date_now < angle2) {
				foc.sector = idx1;
				break;
			}
		}
	}
	// 3. 分段插值，映射到电角度
	uint16_t idx1 = foc.sector;
	uint16_t idx2 = (foc.sector + 1) % SECTOR_TOTAL_COUNT;
	uint16_t angle1 = Flash_Read_SectorAngle(idx1);
	uint16_t angle2 = Flash_Read_SectorAngle(idx2);
	int16_t scope = angle2 - angle1;
	if (scope < 0) scope += ENCODER_MAX_VALUE;
	foc.scope = scope;

	// 计算插值量（避免除0，保证周期性）
	int16_t angle_diff = PID.Mt6816_date_now - angle1;
	if (angle_diff < 0) angle_diff += ENCODER_MAX_VALUE;

	// “每扇区256点分辨率”，电角度一圈0~1023，(foc.sector%4)*256是电周期定位（如四极电机）
	foc.angle = (angle_diff * 256 / foc.scope) + ((foc.sector % 4) * 256);
	foc.angle &= 0x03FF; // 保证0~1023
	return foc.angle;
}
void FOC_Ctrl(int16_t Iq, int16_t Id, uint16_t x)
{
	coil_b.conver = (x+foc.lead_angle) & 0x03FF;                   /* B 相 */
	coil_a.conver = (coil_b.conver + 256) & 0x03FF;  /* A 相超前 90° */

	/* 2. 正弦值 ±4096 */
	coil_a.sin_data = sin_pi_m2[coil_a.conver];
	coil_b.sin_data = sin_pi_m2[coil_b.conver];
	/* 3. 目标电流 → PWM 占空 (0-1023) */

//	dac_max = ((uint32_t)Iq * PWM_MAX) / RATED_I_MA;
//	if (dac_max > PWM_MAX) dac_max = PWM_MAX;              /* 饱和 */

	coil_a.dac_reg = (uint32_t)abs(coil_a.sin_data) * Iq >> 12; /* 保留 10 bit */
	coil_b.dac_reg = (uint32_t)abs(coil_b.sin_data) * Iq >> 12;
	if (coil_a.sin_data > 0)      { AP_H; AM_L; }
	else if (coil_a.sin_data < 0) { AP_L; AM_H; }
	else                          { AP_L; AM_L; }

	if (coil_b.sin_data > 0)      { BP_H; BM_L; }
	else if (coil_b.sin_data < 0) { BP_L; BM_H; }
	else                          { BP_L; BM_L; }
	Set_PWM_A(coil_a.dac_reg);
	Set_PWM_B(coil_b.dac_reg);
	//   Serial_Printf("%d,%d,%d\n",x,Ia,Ib);
}