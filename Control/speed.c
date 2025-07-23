//
// Created by ROG on 2025/7/19.
//

#include "speed.h"
#include "stdlib.h"
struct control PID;
void PID_Init(void )
{
	PID.target_speed = 0;
	PID.speed_ki = 0;
	PID.speed_kp = 0;
	PID.now_speed = 0;
	PID.Leadangle_Kp = 1.5;
	PID.Leadangle_Kd     = 0;
	PID.Leadangle_Ki     = 0;
}
/* 1. 每 50 µs 调用一次：更新速度低通 20KHZ*/
void speed_control_2KHZ(void)
{
	static uint16_t last_cnt ;
    static int cnt_2KHZ=0, diff=0;

	MT6816_ReadAngleDeg_Alt();
       diff =(int)(PID.Mt6816_date_now - last_cnt);
       if (diff >  8192) diff -= 16384;
       if (diff < -8192) diff += 16384;
       last_cnt = PID.Mt6816_date_now;
       PID.Increment += diff ;
       cnt_2KHZ++;
       if (cnt_2KHZ >= 20) {
		       PID.now_speed = (float)PID.Increment /20.0f;
			   PID.Increment = 0;
               cnt_2KHZ = 0;
               Speed_PID_Control(PID.now_speed);
			   Angle_PID();
       }
}
void Speed_PID_Control(float speed_now)
{
	static float bias, last_bias, integral_bias;
	bias =PID.target_speed - speed_now;
	bias = PID.target_speed - speed_now;
// 积分抗风up/溢出
	//if ((bias * last_bias < 0) || abs(PID.speed_out) > 500)
	//else if (abs(integral * ki) < 300.0f)
	if (bias * last_bias < 0 || (PID.speed_out > 500 || PID.speed_out < -500)) {
		integral_bias = 0;
	} else {
		if (integral_bias * PID.speed_ki > -300 && integral_bias * PID.speed_ki < 300) {
			integral_bias += bias;
		}
	}
// PID 公式
	float out =(float)(PID.speed_kp * bias) + PID.speed_ki * integral_bias + PID.speed_kd * (bias - last_bias);

// 限幅
	if (out > 700.0f) out = 700.0f;
	if (out < -700.0f) out = -700.0f;
	last_bias = bias;
	PID.speed_out = (int)out;

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
void Speed_Debug_Init(float kp, float ki, int target_speed)
{
	PID_Init();
	PID.speed_kp = kp;
	PID.speed_ki = ki;
	PID.target_speed = target_speed;
}

void Angle_PID(void)
{
	static float last_bias, integral_bias = 0, bias;
	bias = PID.now_speed;                                                                // 更新数据
	if (integral_bias * PID.Leadangle_Ki < 64 && integral_bias * PID.Leadangle_Ki > -64) // 积分项限幅度
	{
		integral_bias += bias;
	}
	PID.Lead_angle_Out = (int)(PID.Leadangle_Kp * bias + integral_bias * PID.Leadangle_Ki + PID.Leadangle_Kd * (bias - last_bias)); // 计算输出值
	// 输出限幅
	if (PID.Lead_angle_Out > 512) {
		PID.Lead_angle_Out = 512;
	}
	if (PID.Lead_angle_Out < -512) {
		PID.Lead_angle_Out = -512;
	}
	PID.Lead_angle_Out += 1024;
	last_bias = bias;
}


