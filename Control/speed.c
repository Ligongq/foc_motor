//
// Created by ROG on 2025/7/19.
//

#include "speed.h"
control PID;
#define I_TERM_LIM        8000.0f      /* 积分上限 deg/s -> microstep/s */
#define OUT_STEP_LIM         512       /* 最大一次 microstep 增量 (0–1023) */
#define LPF_ALPHA            0.040f     /* 速度 LPF 系数，0.1≈2 kHz 截止 */
static float  i_term      = 0.0f;       /* 积分累加器 */

void PID_Init(void )
{
	PID.target_speed = 0;
	PID.speed_ki = 0;
	PID.speed_kp = 0;
	PID.now_speed = 0;
}
/* 1. 每 50 µs 调用一次：更新速度低通 20KHZ*/
void speed_control_2KHZ(void)
{	static uint16_t last_cnt = 0;
       static uint16_t cnt_2KHZ = 0;
       static float    speed_sum = 0.0f;
       float diff;
       MT6816_ReadAngleDeg_Alt();
       diff = (float)PID.Mt6816_date_now - (float)last_cnt;
       if (diff >  8192.0f) diff -= 16384.0f;
       if (diff < -8192.0f) diff += 16384.0f;
       last_cnt = PID.Mt6816_date_now;

       speed_sum += diff ;
       cnt_2KHZ++;

       if (cnt_2KHZ >= 20) {
               if (fabsf(speed_sum) < 1e-3f) PID.now_speed = 0.0f;
               else PID.now_speed = speed_sum / 20.0f;
               speed_sum = 0.0f;
               cnt_2KHZ = 0;
               Speed_PID_Control(PID.now_speed);
       }
}
void Speed_PID_Control(float speed_now)
{
	static float last_bias = 0;
	static float integral = 0;

	float bias = PID.target_speed - speed_now;
	float kp = PID.speed_kp;
	float ki = PID.speed_ki;
	float kd = PID.speed_kd;

	if ((bias * last_bias < 0) || fabsf(PID.speed_out) > 500.0f)
		integral = 0;
	else if (fabsf(integral * ki) < 300.0f)
		integral += bias;

	float out = kp * bias + ki * integral + kd * (bias - last_bias);

	if (out > 700.0f) out = 700.0f;
	if (out < -700.0f) out = -700.0f;

	PID.speed_out = (int)out;
	last_bias = bias;
}

//		static uint32_t now_time,last_time=0;
//		static uint16_t angle_last,angle_date_last=0;
//		static float speed_err,speed_err_sum=0;
//		last_time = now_time;
//		now_time = HAL_GetTick();
//		uint32_t time_dt = (now_time -last_time);
//		calc_speed_50us(PID.Mt6816_date_now);//use date improve
//		angle_date_last = PID.Mt6816_date_now;
//		//---------PI-------
//        speed_err = PID.target_speed - PID.now_speed;
//		speed_err_sum += (speed_err * time_dt*0.001);
//		if (speed_err_sum > SPEED_ERR_SUM_MAX) speed_err_sum = SPEED_ERR_SUM_MAX;
//		else if (speed_err_sum < SPEED_ERR_SUM_MIN) speed_err_sum = SPEED_ERR_SUM_MIN;
//		float out = PID.speed_kp * speed_err + PID.speed_ki * speed_err_sum;
//        return  (uint16_t)out;}

//////////////////////////////////////////
void Speed_Debug_Init(float kp, float ki, float target_speed)
{
	PID_Init();
	PID.speed_kp = kp;
	PID.speed_ki = ki;
	PID.target_speed = target_speed;
}


