#include "stepmotor_utils.h"
#include <stdio.h>

/* 与主 PWM 周期保持一致 */
#ifndef PWM_MAX
#define PWM_MAX  1023U
#endif

/* 1) mA → DAC */
uint16_t mA_to_dac(uint16_t i_ma)
{
	uint32_t d = (uint32_t)i_ma * PWM_MAX / (uint32_t)(MOTOR_I_RATED_A * 1000);
	return (d > PWM_MAX) ? PWM_MAX : (uint16_t)d;
}

/* 2) mA → VREF (V) : Vref = Ipeak × K × Rsense */
float mA_to_vref(uint16_t i_ma)
{
	float i_a = i_ma / 1000.0f;                     /* mA→A */
	return i_a * (DRV_K_MVPERA / 1000.0f) * (RSENSE_MOHM / 1000.0f);
}

/* 3) mA → I_peak (A) : I_peak 理论=设定值×(π/2)/√2≈1.11×Irms
   但在恒流斩波里直接等于设定峰值 -> i_ma/1000 */
float mA_to_Ipeak(uint16_t i_ma)
{
	return i_ma / 1000.0f;
}

/* 4) mA → Torque (N·m) : τ = Kt × Iq (假设 d 轴≈0) */
float mA_to_torque(uint16_t i_ma)
{
	return (i_ma / 1000.0f) * MOTOR_KT_NM_PER_A;
}

/* 5) 打印调试信息 */
void debug_print_all(uint16_t i_ma)
{
	float vref  = mA_to_vref(i_ma);
	float ipeak = mA_to_Ipeak(i_ma);
	float tq    = mA_to_torque(i_ma);

	printf("Set %u mA  ->  VREF=%.3f V  I_peak=%.2f A  Torque=%.3f N·m\r\n",
	       i_ma, vref, ipeak, tq);
}
