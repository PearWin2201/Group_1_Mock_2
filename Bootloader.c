/*
 * Copyright 2016-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    Bootloader.c
 * @brief   Application entry point.
 */
#include "MKL46Z4.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */

#define FCMD_WRITE_WORD   0X06U
#define FCMD_ERASE_SECTOR 0X09U
#define GET_SECTOR_NUMBER(ADDR)   ((uint32_t)(ADDR)/1024U)

uint32_t Flash_Read(uint32_t Address)
{

}

void Flash_WriteWord(uint32_t Address, uint32_t Data)
{
	//Wait previous commands complete
	while ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);

	//
	if  (((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U) || \
		 ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U))
		{
			FTFA->FSTAT = 0x30;
		}

	//Write command and parameters
	FTFA->FCCOB0 = FCMD_WRITE_WORD;
	FTFA->FCCOB1 = (uint8_t)(Address >> 16);
	FTFA->FCCOB2 = (uint8_t)(Address >> 8);
	FTFA->FCCOB3 = (uint8_t)Address;
	FTFA->FCCOB4 = (uint8_t)(Data >> 24);
	FTFA->FCCOB5 = (uint8_t)(Data >> 16);
	FTFA->FCCOB6 = (uint8_t)(Data >> 8);
	FTFA->FCCOB7 = (uint8_t)Data;

	//Launch the command
	FTFA->FSTAT = 0x80;
}

void Flash_EraseSector(uint8_t SectorNum)
{
	uint32_t Address = SectorNum * 1024U;

	if (SectorNum <= 1)
	{
		return;
	}

	//Wait previous commands complete
	while ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);

	//
	if  (((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U) || \
		((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U))
		{
			FTFA->FSTAT = 0x30;
		}

	//Write command and parameters
	FTFA->FCCOB0 = FCMD_ERASE_SECTOR;
	FTFA->FCCOB1 = (uint8_t)(Address >> 16);
	FTFA->FCCOB2 = (uint8_t)(Address >> 8);
	FTFA->FCCOB3 = (uint8_t)Address;

	//Launch the command
	FTFA->FSTAT = 0x80;
}

void main(void)
{
	Flash_WriteWord(0xF000, 0x12345678);
	Flash_WriteWord(0xF100, 0x12345678);
	Flash_EraseSector(60);
}
