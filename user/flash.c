//
// Created by ROG on 2025/7/22.
//

#include "flash.h"
#include "stm32f1xx_hal.h"
void Write_CaliData_Array(uint16_t *data, uint32_t num)
{
	HAL_FLASH_Unlock();
	// 1. 擦除所需页
	uint32_t pageError = 0;
	FLASH_EraseInitTypeDef eraseInit;
	eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInit.PageAddress = FLASH_LUT_START_ADDR;
	eraseInit.NbPages = (FLASH_LUT_END_ADDR - FLASH_LUT_START_ADDR + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
	HAL_FLASHEx_Erase(&eraseInit, &pageError);

	// 2. 顺序写入每个16位数据
	uint32_t address = FLASH_LUT_START_ADDR;
	for (uint32_t i = 0; i < num; ++i, address += 2) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data[i]);
	}
	HAL_FLASH_Lock();
}
uint8_t Is_LUT_Valid(void)
{
	uint16_t head = *(__IO uint16_t *)FLASH_LUT_START_ADDR;
	return (head != 0xFFFF) ? 1 : 0;
}

void Flash_Write_Zero(uint16_t zero)
{
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_LUT_ZERO_ADDR, zero);
	HAL_FLASH_Lock();
}

uint16_t Flash_Read_SectorAngle(int addr)
{
	if (addr >= FLASH_LUT_NUM) return 0xFFFF;
	uint32_t address = FLASH_LUT_START_ADDR + addr * 2;
	return *(__IO uint16_t *)address;
}

uint16_t Flash_Read_Original_Point(void)
{
	return *(__IO uint16_t *)FLASH_LUT_ZERO_ADDR;
}
