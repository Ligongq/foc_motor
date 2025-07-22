//
// Created by ROG on 2025/7/20.
//

#ifndef __DEBUG_H_
#define __DEBUG_H_

#include "main.h"
#include "usart.h"     // CubeMX 生成
#include "dma.h"
#include "string.h"
#include <stdarg.h>
#include <stdio.h>
#include "key.h"
#include "start.h"
void uart1_printf(const char *fmt, ...);       /* 非阻塞输出 */
void debug_poll(void);                      /* 主循环里调，解析指令 */
#endif //MOTOR_FOC_DEBUG_H
