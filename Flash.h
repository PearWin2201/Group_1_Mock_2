/*
 * Flash.h
 *
 *  Created on: Oct 18, 2023
 *      Author: DELL
 */

#ifndef FLASH_H_
#define FLASH_H_

#include"MKL46Z4.h"

#define FCMD_WRITE_WORD       (0x06u)
#define FCMD_ERASE_SECTOR     (0x09u)
#define FIRM_WARE_RESET_HANDLER_OFFSET  4u
#define Null  0
#define FIRM_WARE_BASE_ADDR  0x30000

void Flash_WriteWord(uint32_t word, uint32_t address);

void Flash_EraseSector(uint8_t sectorNum);

void Run_In_Application(void);

#endif /* FLASH_H_ */
