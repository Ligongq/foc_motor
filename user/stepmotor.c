//
// Created by ROG on 2025/7/11.
//
//
// Created by ROG on 2025/7/10.
//

#include "stepmotor.h"
#include <stdlib.h>
 Coil_Typedef coil_a = {0,0,0};
 Coil_Typedef coil_b = {0,0,0};
#define PWM_MAX     1023  // PWM周期

#define RATED_I_MA   1700     /* 3.3 V / (10×0.30) ≈ 1100 mA */

uint32_t dac_max=0;

void Motor_Sleep(void)
{
	Set_PWM_A(0);	Set_PWM_B(0);
	AP_L;	AM_L;
	BP_L;	BM_L;
}
void Motor_Brake(void)
{
	Set_PWM_A(0);	Set_PWM_B(0);
	AP_H;	AM_H;
	BP_H;	BM_H;
}
void Motor_Test_Rotate(void)
{   Motor_Sleep();
	for (int step = 0; step < 400; step++) // 转动 500 步
	{
		switch (step % 4) {
			case 0: // A+
				AP_H;
				AM_L;
				BP_L;	BM_L;
				break;
			case 1: // B+
				BP_H;
				BM_L;
				AP_L;	AM_L;
				break;
			case 2: // A-
				AP_L;
				AM_H;
				BP_L;	BM_L;
				break;
			case 3: // B-
				BP_L;
				BM_H;
				AP_L;	AM_L;
				break;
		}
		HAL_Delay(3);
	}
}


void Motor_MicroStep(uint32_t divide, int32_t elec_ma)
{

	/* 1. 索引 */
	coil_b.conver = divide & 0x03FF;                   /* B 相 */
	coil_a.conver = (coil_b.conver + 256) & 0x03FF;  /* A 相超前 90° */

	/* 2. 正弦值 ±4096 */
	coil_a.sin_data = sin_pi_m2[coil_a.conver];
	coil_b.sin_data = sin_pi_m2[coil_b.conver];

	/* 3. 目标电流 → PWM 占空 (0-1023) */
	if (elec_ma < 0) elec_ma = -elec_ma;
	 dac_max = ((uint32_t)elec_ma * PWM_MAX) / RATED_I_MA;
	if (dac_max > PWM_MAX) dac_max = PWM_MAX;              /* 饱和 */

	coil_a.dac_reg = (uint32_t)abs(coil_a.sin_data) * dac_max >> 12; /* 保留 10 bit */
	coil_b.dac_reg = (uint32_t)abs(coil_b.sin_data) * dac_max >> 12;

	/* 4. 先方向后 PWM */
	if (coil_a.sin_data > 0)      { AP_H; AM_L; }
	else if (coil_a.sin_data < 0) { AP_L; AM_H; }
	else                          { AP_L; AM_L; }

	if (coil_b.sin_data > 0)      { BP_H; BM_L; }
	else if (coil_b.sin_data < 0) { BP_L; BM_H; }
	else                          { BP_L; BM_L; }
	Set_PWM_A(coil_a.dac_reg);
	Set_PWM_B(coil_b.dac_reg);

}