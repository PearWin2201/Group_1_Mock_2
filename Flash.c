/*
 * Flash.c
 *
 *  Created on: Oct 18, 2023
 *      Author: DELL
 */
#include "Flash.h"

typedef void (*FUNC_POINTER)(void);
FUNC_POINTER Firmware_Reset_Handler = Null;
uint32_t Firmware_Main_Stack_Pointer = 0;
uint32_t Firmware_Reset_Handler_Address = 0;

void Flash_WriteWord(uint32_t Word, uint32_t address)
{
	while( (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0u);

	if( ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0u) || ((FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) != 0u))
	{
		FTFA->FSTAT = 0x30u;
	}

	// write command
	FTFA->FCCOB0 = FCMD_WRITE_WORD;
	FTFA->FCCOB1 = (uint8_t)(address >> 16);
	FTFA->FCCOB2 = (uint8_t)(address >> 8);
	FTFA->FCCOB3 = (uint8_t)(address);
	FTFA->FCCOB7 = (uint8_t)(Word);
	FTFA->FCCOB6 = (uint8_t)(Word >> 8);
	FTFA->FCCOB5 = (uint8_t)(Word >> 16);
	FTFA->FCCOB4 = (uint8_t)(Word >> 24);

	FTFA->FSTAT  = 0x80u;
}

void Flash_EraseSector(uint8_t sectorNum)
{
	uint32_t address = sectorNum * 1024u;
	while( (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0u);

	if(((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0u)|| ((FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK )!= 0u))
	{
		FTFA->FSTAT = 0x30u;
	}

	// write command
	FTFA->FCCOB0 = FCMD_ERASE_SECTOR;
	FTFA->FCCOB1 = (uint8_t)(address >> 16);
	FTFA->FCCOB2 = (uint8_t)(address >> 8);
	FTFA->FCCOB3 = (uint8_t)(address);

	FTFA->FSTAT  = 0x80u;

}

void Run_In_Application(void)
{
	//set main stack pointer
	Firmware_Main_Stack_Pointer = *(uint32_t*)(FIRM_WARE_BASE_ADDR);
	__set_MSP(Firmware_Main_Stack_Pointer);

//disable interupt and turn off system tick
	 __disable_irq();
//clear interrupt enable
	SysTick->CTRL = 0 ;
	SysTick->LOAD = 0;


	//Jump to application
	Firmware_Reset_Handler_Address = *(uint32_t*)(FIRM_WARE_BASE_ADDR + FIRM_WARE_RESET_HANDLER_OFFSET);
	Firmware_Reset_Handler = (FUNC_POINTER)Firmware_Reset_Handler_Address;
	Firmware_Reset_Handler();

}

