//
// Created by ROG on 2025/7/22.
//

#ifndef __FLASH_H_
#define __FLASH_H_
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
#define FLASH_LUT_START_ADDR     ((uint32_t)0x0801F800)   // 128K flash, 最后 2K, 可改
#define FLASH_LUT_NUM            200          // LUT存储点数（按你自己的需求）
#define FLASH_LUT_ZERO_ADDR      (FLASH_LUT_START_ADDR + FLASH_LUT_NUM * 2)  // 零点地址

#define FLASH_LUT_END_ADDR       (FLASH_LUT_ZERO_ADDR + 2) // 末地址
uint8_t Is_LUT_Valid(void);
void Write_CaliData_Array(uint16_t *data, uint32_t num);
uint16_t Flash_Read_SectorAngle(int addr);
void Flash_Write_Zero(uint16_t zero);
uint16_t Flash_Read_Original_Point(void);
#endif /* __FLASH_CALI_H */


