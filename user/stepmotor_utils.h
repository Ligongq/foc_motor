#ifndef __STEPMOTOR_UTILS_H_
#define __STEPMOTOR_UTILS_H_
#include "main.h"
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdint.h"
#include "start.h"
#include "math.h"
#include "sin.h"
/* ----------- 驱动与电机参数 ---------------- */
#define RSENSE_MOHM      300       /* R14/R17 = 0.30 Ω → 300 mΩ     */
#define DRV_K_MVPERA     540       /* TB67H450: 0.54 Ω ⇒ 540 mV/A   */

#define MOTOR_T_NM       0.43f     /* 铭牌保持力矩 0.43 N·m (at 2 A)*/
#define MOTOR_I_RATED_A  2.0f      /* 额定相电流 2 A                */
#define MOTOR_KT_NM_PER_A (MOTOR_T_NM / MOTOR_I_RATED_A)   /* ≈0.215 */

/* ----------- API --------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* 1) 设定电流(mA)  →  DAC占空(0-1023) */
uint16_t mA_to_dac(uint16_t i_ma);

/* 2) 设定电流(mA)  →  理论 VREF (Volt) */
float    mA_to_vref(uint16_t i_ma);

/* 3) 设定电流(mA)  →  线圈峰值电流(A) (理论值) */
float    mA_to_Ipeak(uint16_t i_ma);

/* 4) 设定电流(mA)  →  理论输出扭矩 (N·m) ≈ Kt·Iq */
float    mA_to_torque(uint16_t i_ma);

/* 5) 调试打印：电流 / VREF / Ipk / Torque */
void     debug_print_all(uint16_t i_ma);

#ifdef __cplusplus
}
#endif

#endif /* __STEPMOTOR_UTILS_H */
