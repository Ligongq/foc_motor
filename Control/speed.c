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

static float calc_speed_now(uint16_t now, uint16_t last, float dt_s)
{
	int diff = (int)now - (int)last;
	if (diff > 8192) diff -= 16384;
	else if (diff < -8192) diff += 16384;
	// 每个脉冲对应的角度
	float deg = (float)diff * (360.0f / 16384.0f);  // 单位：度
	// 角速度（度/秒）
	return deg / dt_s;
}
uint16_t Speed_PID_Control(void)
{
		static uint32_t now_time,last_time;
		static float angle_last,speed_err,speed_err_sum;
		last_time = now_time;
		now_time = HAL_GetTick();
		float time_dt = (float)(now_time -last_time)/1000.0f;
		PID.now_speed = calc_speed_now(PID.angle_get_now,angle_last,time_dt);
		angle_last = PID.angle_get_now;
        speed_err = PID.target_speed - PID.now_speed;
		speed_err_sum += speed_err * time_dt;
		if (speed_err_sum > SPEED_ERR_SUM_MAX) speed_err_sum = SPEED_ERR_SUM_MAX;
		else if (speed_err_sum < SPEED_ERR_SUM_MIN) speed_err_sum = SPEED_ERR_SUM_MIN;
		float out = PID.speed_kp * speed_err + PID.speed_ki * speed_err_sum;
        return  (uint16_t)out;
}


