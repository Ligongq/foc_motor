//
// Created by ROG on 2025/7/19.
//

#include "speed.h"

static float calc_speed_rpm(uint16_t now, uint16_t last, float dt_s)
{
        int diff = (int)now - (int)last;
        if (diff > 8192) diff -= 16384;
        else if (diff < -8192) diff += 16384;
        float rev = diff / 16384.0f;
        return (rev / dt_s) * 60.0f;
}

void SpeedPID_Init(SpeedPID_t *pid, float target_rpm, float kp, float ki,
                   uint16_t amp_min, uint16_t amp_max)
{
        pid->kp = kp;
        pid->ki = ki;
        pid->target_rpm = target_rpm;
        pid->integral = 0.0f;
        pid->amp_min = amp_min;
        pid->amp_max = amp_max;
        pid->amp = amp_min;
}

uint16_t SpeedPID_Update(SpeedPID_t *pid, float cur_rpm, float dt_s)
{
        float err = pid->target_rpm - cur_rpm;
        pid->integral += err * dt_s;
        float out = pid->kp * err + pid->ki * pid->integral;
        if (out < pid->amp_min) out = pid->amp_min;
        if (out > pid->amp_max) out = pid->amp_max;
        pid->amp = (uint16_t)out;
        return pid->amp;
}

void Speed_ClosedLoopDemo(void)
{
        char buf[64];
        SpeedPID_t pid;
        SpeedPID_Init(&pid, 30.0f, 2.0f, 50.0f, 100, 1000);

        HAL_SYSTICK_Config(SystemCoreClock / 20000); // 50us tick

        uint16_t micro = 0;
        uint16_t last_pos = MT6816_ReadRaw_Alt();
        uint32_t last_ms = HAL_GetTick();
        uint32_t last_ctrl = last_ms;

        for (;;) {
                uint32_t now = HAL_GetTick();
                if (now - last_ms >= 1) {
                        last_ms = now;
                        micro = (micro + 8) & 0x03FF;
                        Motor_MicroStep(micro, pid.amp);
                }
                if (now - last_ctrl >= 10) {
                        last_ctrl = now;
                        uint16_t pos = MT6816_ReadRaw_Alt();
                        float rpm = calc_speed_rpm(pos, last_pos, 0.01f);
                        last_pos = pos;
                        SpeedPID_Update(&pid, rpm, 0.01f);
                        sprintf(buf, "tar=%.1f rpm=%.1f amp=%u\r\n", pid.target_rpm, rpm, pid.amp);
                        HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 200);
                }
        }
}

void Speed_Test(void)
{
        SpeedPID_t pid;
        SpeedPID_Init(&pid, 30.0f, 1.0f, 5.0f, 0, 1000);
        const float samples[] = {0, 10, 20, 25, 30, 35, 30};
        char buf[64];
        for (unsigned i = 0; i < sizeof(samples)/sizeof(samples[0]); ++i) {
                uint16_t amp = SpeedPID_Update(&pid, samples[i], 0.01f);
                sprintf(buf, "cur=%.1f amp=%u\r\n", samples[i], amp);
                HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), 100);
        }
}

