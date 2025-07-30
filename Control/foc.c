//
// Created by ROG on 2025/7/22.
//

#include "foc.h"
#include "stdlib.h"
uint16_t Scope[200]={0};
#define PWM_MAX     1023  // PWM周期
#define RATED_I_MA   1600     /* 3.3 V / (10×0.30) ≈ 1100 mA */
struct FOC_parameter_t foc={0};
void Foc_Init(void)
{
	foc.angle  = 0;
	foc.scope  = 0;
	foc.sector = 0;
	for (uint8_t i = 0; i < SECTOR_TOTAL_COUNT; i++) {
		Scope[i] = Flash_Read_SectorAngle(i + 1) - Flash_Read_SectorAngle(i);
		if (Scope[i] > SECTOR_TOTAL_COUNT) {
			Scope[i] += ENCODER_MAX_VALUE;
			foc.offset = i;
		}
	//	uart1_printf("%d\n",Scope[i]);
	//	HAL_Delay(10);
	}
}
void Sector_tracking(void)
{
	uint16_t temp = (PID.Mt6816_date_now / 82 + foc.offset) % SECTOR_TOTAL_COUNT; // 获取扇区大致位置
	for (int i = temp - 1; i <= temp + 1; i++)                                                       // 在这个粗略计算的扇区的上一个和下一个比较一下看当前的角度是不是在这两个之间从而确定扇区
	{
		if (Flash_Read_SectorAngle(i + 1) > Flash_Read_SectorAngle(i))
		{
			if (PID.Mt6816_date_now >= Flash_Read_SectorAngle(i) && PID.Mt6816_date_now <= Flash_Read_SectorAngle(i + 1)) // 在这两个角度数据之间 那么就是在这个扇区
			{
				foc.sector = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT; // 让扇区序号的大小在200以内
				break;                                                      // 判断完毕，退出FOR
			}
			// 以下处理特殊情况，也就是编码器跨越零点的时候
		}
		else if (Flash_Read_SectorAngle(i + 1) < Flash_Read_SectorAngle(i)) // 跨越零点的时候
		{
			if (PID.Mt6816_date_now >= Flash_Read_SectorAngle(i) && PID.Mt6816_date_now <= (Flash_Read_SectorAngle(i + 1) + ENCODER_MAX_VALUE)) {
				foc.sector = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT; // 让扇区序号的大小在200以内
				break;                                                      // 判断完毕，退出循环
			}
		}
	}
	//-----------------------------------------------------------准确找到当前角度在哪个扇区------------------------------------------------------

	//-----------------------------------------------------------计算出当前的实际角度对应的电角度-------------------------------------------------
	foc.scope = Scope[foc.sector];
	if (Flash_Read_SectorAngle(foc.sector + 1) > Flash_Read_SectorAngle(foc.sector))
	{
		foc.angle = (PID.Mt6816_date_now - Flash_Read_SectorAngle(foc.sector)) * 256 / foc.scope;//+ (foc.sector % 4) * 256;
	}
	else
	{ // 处理零点跨越的情况
		if (PID.Mt6816_date_now > 16200) {
			foc.angle = (PID.Mt6816_date_now - Flash_Read_SectorAngle(foc.sector)) * 256 / foc.scope ;//+ (foc.sector % 4) * 256;
		} else
		{
			foc.angle = (16384 - Flash_Read_SectorAngle(foc.sector) + PID.Mt6816_date_now) * 256 / foc.scope ;//+ (foc.sector % 4) * 256;
		}
	}

}

void FOC_Ctrl(int16_t Iq, int16_t Id)
{
	int cost,sint;
	int Ia,Ib,x;
	x = (foc.angle + 256 * (foc.sector % 4) + foc.lead_angle) % 1024;
	sint = sin_pi_m2[x];
	cost = sin_pi_m2[(x+256)%1024];
	Ia = (Id * cost - Iq * sint) / 4096;//park反变换
	Ib = (Iq * cost + Id * sint) / 4096;
	if (Ia > 0)      { AP_L; AM_H; }
	else if (Ia < 0) { Ia = -Ia;AP_H; AM_L; }
	else                          { AP_L; AM_L; }

	if (Ib > 0)      { BP_L; BM_H; }
	else if (Ib < 0) {Ib = -Ib; BP_H; BM_L; }
	else                          { BP_L; BM_L; }
	Set_PWM_A(Ia);
	Set_PWM_B(Ib);

}