#include "flash.h"

#define FLASH_USER_START_ADDR 0x0801FC00
#define FLASH_PAGE_ADDR 0x0801FC00

static uint8_t flash_page_erased = 0;

void Flash_Init(void)
{
	HAL_FLASH_Unlock();
	HAL_FLASH_Lock();
	flash_page_erased = 0;
}

void Flash_ReadIDs(uint8_t *ids)
{
	uint32_t addr = FLASH_USER_START_ADDR;
	for(uint8_t i = 0; i < FLASH_ID_COUNT; i++)
	{
		for(uint8_t j = 0; j < FLASH_ID_SIZE; j++)
		{
			ids[i * FLASH_ID_SIZE + j] = *(__IO uint8_t*)addr;
			addr++;
		}
	}
}

void Flash_WriteID(uint8_t index, uint8_t *id)
{
	if(index >= FLASH_ID_COUNT) return;
	
	HAL_FLASH_Unlock();
	
	if(!flash_page_erased)
	{
		FLASH_EraseInitTypeDef EraseInitStruct;
		uint32_t SectorError = 0;
		
		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.PageAddress = FLASH_PAGE_ADDR;
		EraseInitStruct.NbPages = 1;
		
		HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
		flash_page_erased = 1;
	}
	
	uint32_t addr = FLASH_USER_START_ADDR + index * FLASH_ID_SIZE;
	uint32_t data = ((uint32_t)id[3] << 24) | ((uint32_t)id[2] << 16) | ((uint32_t)id[1] << 8) | id[0];
	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data);
	
	HAL_FLASH_Lock();
}

void Flash_DeleteID(uint8_t index)
{
	if(index >= FLASH_ID_COUNT) return;
	
	HAL_FLASH_Unlock();
	
	uint32_t addr = FLASH_USER_START_ADDR + index * FLASH_ID_SIZE;
	uint32_t data = 0xFFFFFFFF;
	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data);
	
	HAL_FLASH_Lock();
}

uint8_t Flash_FindID(uint8_t *id)
{
	uint32_t addr = FLASH_USER_START_ADDR;
	
	for(uint8_t i = 0; i < FLASH_ID_COUNT; i++)
	{
		uint8_t match = 1;
		for(uint8_t j = 0; j < FLASH_ID_SIZE; j++)
		{
			if(*(__IO uint8_t*)addr != id[j])
			{
				match = 0;
				break;
			}
			addr++;
		}
		if(match) return i;
	}
	return 0xFF;
}

void Flash_ClearAll(void)
{
	HAL_FLASH_Unlock();
	
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError = 0;
	
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_PAGE_ADDR;
	EraseInitStruct.NbPages = 1;
	
	if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
		HAL_FLASH_Lock();
		return;
	}
	
	flash_page_erased = 0;
	HAL_FLASH_Lock();
}
