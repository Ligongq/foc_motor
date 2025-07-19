//
// Created by ROG on 2025/7/19.
//

#ifndef __SPEED_H_
#define __SPEED_H_
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdint.h"
#include "start.h"
#include "math.h"
#include "sin.h"
#include "stepmotor_utils.h"
#include "stepmotor.h"
#include "mt6816.h"
#include <stdio.h>
#include <string.h>

typedef struct {
        float kp;
        float ki;
        float target_rpm;
        float integral;
        uint16_t amp_min;
        uint16_t amp_max;
        uint16_t amp;
} SpeedPID_t;

void SpeedPID_Init(SpeedPID_t *pid, float target_rpm, float kp, float ki,
                   uint16_t amp_min, uint16_t amp_max);
uint16_t SpeedPID_Update(SpeedPID_t *pid, float cur_rpm, float dt_s);

void Speed_ClosedLoopDemo(void);
void Speed_Test(void);
#endif //MOTOR_FOC_SPEED_H
