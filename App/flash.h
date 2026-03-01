#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"

#define FLASH_ID_COUNT 10
#define FLASH_ID_SIZE 4
#define FLASH_TOTAL_SIZE (FLASH_ID_COUNT * FLASH_ID_SIZE)

#define FLASH_USER_START_ADDR 0x08060000
#define FLASH_USER_END_ADDR 0x0807FFFF

void Flash_Init(void);
void Flash_ReadIDs(uint8_t *ids);
void Flash_WriteID(uint8_t index, uint8_t *id);
void Flash_DeleteID(uint8_t index);
uint8_t Flash_FindID(uint8_t *id);
void Flash_ClearAll(void);

#endif
