//
// Created by ROG on 2025/7/19.
//

#include "speed.h"
control PID;
void PID_Init(void )
{
	PID.target_speed = 0;
	PID.speed_ki = 0;
	PID.speed_kp = 0;
	PID.now_speed = 0;
}
static float calc_speed_now(uint16_t now_cnt, uint16_t last_cnt,uint16_t dt_ms)   // 这里明确写 ms
{
	static int32_t diff_sum_cnt  = 0;   // 累积的 Δcount
	static uint16_t win_ms       = 0;   // 累积的窗口时间（ms）
	static uint8_t  sample_cnt   = 0;   // 累积的样本数
	static float    speed_deg_s  = 0.0f;
	int32_t diff = (int32_t)now_cnt - (int32_t)last_cnt;
	if (diff >  8192) diff -= 16384;
	if (diff < -8192) diff += 16384;
	/* 2. 累加进窗口 */
	diff_sum_cnt += diff;
	win_ms += dt_ms;
	/* 3. 到窗口长度就结算一次速度 */
	if (++sample_cnt >= 10)
	{
		float dt_s   = win_ms * 0.001f;          // ms → s
		float deg    = diff_sum_cnt * (360.0f / 16384.0f);
		speed_deg_s  = deg / dt_s;               // 单位：deg/s
		/* 4. 复位窗口 */
		diff_sum_cnt = 0;
		win_ms       = 0;
		sample_cnt   = 0;
	}
	return speed_deg_s;      // **始终返回最近一次的结果**
}
uint16_t Speed_PID_Control(void)
{
		static uint32_t now_time,last_time=0;
		static uint16_t angle_last,angle_date_last=0;
		static float speed_err,speed_err_sum=0;
		last_time = now_time;
		now_time = HAL_GetTick();
		uint32_t time_dt = (now_time -last_time);
		PID.now_speed = calc_speed_now(PID.Mt6816_date_now,angle_date_last,time_dt);//use date improve
		if (fabsf(PID.now_speed) < 5.0f) PID.now_speed = 0.0f;
		angle_date_last = PID.Mt6816_date_now;
		//---------PI-------
        speed_err = PID.target_speed - PID.now_speed;
		speed_err_sum += (speed_err * time_dt*0.001);
		if (speed_err_sum > SPEED_ERR_SUM_MAX) speed_err_sum = SPEED_ERR_SUM_MAX;
		else if (speed_err_sum < SPEED_ERR_SUM_MIN) speed_err_sum = SPEED_ERR_SUM_MIN;
		float out = PID.speed_kp * speed_err + PID.speed_ki * speed_err_sum;
        return  (uint16_t)out;
}

//////////////////////////////////////////
void Speed_Debug_Init(float kp, float ki, float target_speed)
{
	PID_Init();
	PID.speed_kp = kp;
	PID.speed_ki = ki;
	PID.target_speed = target_speed;
}


