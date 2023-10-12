/*
 * driver.c
 *
 *  Created on: Oct 11, 2023
 *      Author: ADMIN
 */

#include "driver/driver.h"

static CallbackType s_gCallback = NULL;


void BUTTON_HANDLE(uint8_t pinnum)
{
	// callback function

}

void PORT_Init(PORT_Type * PORTx, uint8_t pinnum, const PortConfigType * Config)
{
	/* Configure Multiplexing */
	PORTx->PCR[pinnum] &= ~PORT_PCR_MUX_MASK;
	PORTx->PCR[pinnum] |= PORT_PCR_MUX(Config->mux);

	/* Configure Pull Control */
	PORTx->PCR[pinnum] &= ~PORT_PCR_PE_MASK;
	PORTx->PCR[pinnum] &= ~PORT_PCR_PS_MASK;
	PORTx->PCR[pinnum] |= (Config->pull << 0U);

	/* Configure Interrupt Control */
	PORTx->PCR[pinnum] &= ~PORT_PCR_IRQC_MASK;
	PORTx->PCR[pinnum] |= PORT_PCR_IRQC(Config->interrupt);

	if (PORT_INTERRUPT_DISABLE != Config->interrupt)
	{
		// 2. NVIC - Enable Interrupt.
		if (PORTx == PORTA)
		{
			NVIC_EnableIRQ(PORTA_IRQn);
		}
		if (PORTx == PORTC || PORTx == PORTD)
		{
			NVIC_EnableIRQ(PORTC_PORTD_IRQn);
		}

		s_gCallback = Config->callback;
	}
}

void Clock_LED_Init(void)
{
	// Enable Clock PORTD
	SIM->SCGC5 |= SIM_SCGC5_PORTD(1U);
}

void Clock_PIT_Init(void)
{
	// 1. Enable Clock for PIT
	/* Update Bus Clock  = 1MHz */
	MCG->C1 &= ~MCG_C1_CLKS_MASK;
	MCG->C1 |= MCG_C1_CLKS(1U);
	MCG->C2 |= MCG_C2_IRCS(1U);
	SIM->CLKDIV1 &= ~SIM_CLKDIV1_OUTDIV1_MASK;
	SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(1U);
	SIM->CLKDIV1 &= ~SIM_CLKDIV1_OUTDIV4_MASK;

	/* Enable Clock Gate for PIT */
	SIM->SCGC6 |= SIM_SCGC6_PIT(1U);
}

void PIT_Init(void) // param: channel? 500ms? interrupt? debug mode? chain mode
{
	// 1.1. Enable PIT Module - MCR
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;

	// 2. Calculate & program reload value
	PIT->CHANNEL[0].LDVAL = 499999U;

	// 3. Enable Interrupt
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE(1U);
	NVIC_EnableIRQ(PIT_IRQn);

	// 4. Debug mode or not?
	// 5. Chain mode or not?
}

void PIT_StartTimer(uint8_t channel)
{
	// Enable Timer
	PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN(1U);
}

uint32_t Flash_Read(uint32_t Address)
{
	//Wait previous commands complete
	while ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);

	//Access Error and Protection Violation Check
	if  (((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U) || \
		 ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U))
		{
			FTFA->FSTAT = 0x30;
		}

	//Read Commands and Parameter
	FTFA->FCCOB0 = FCMD_READ;
	FTFA->FCCOB1 = (uint8_t)(Address >> 16);
	FTFA->FCCOB2 = (uint8_t)(Address >> 8);
	FTFA->FCCOB3 = (uint8_t)Address;
	FTFA->FCCOB8 = 0U;

	//Launch the commands
	FTFA->FSTAT = 0x80;

	uint32_t Data_Read = 0;
	Data_Read |= ((uint32_t)(FTFA->FCCOB4) << 24);
	Data_Read |= ((uint32_t)(FTFA->FCCOB5) << 16);
	Data_Read |= ((uint32_t)(FTFA->FCCOB6) << 8);
	Data_Read |= (uint32_t)(FTFA->FCCOB7);
	return Data_Read;
}

void Flash_WriteWord(uint32_t Address, uint32_t Data)
{
	//Wait previous commands complete
	while ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);

	//Access Error and Protection Violation Check
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

void UART_Init(void)
{
	//Clock (Enable clock PORTA / UART0 / 4MHz)
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1U);
	SIM->SCGC4 |= SIM_SCGC4_UART0(1U);
	MCG->C2 |= MCG_C2_IRCS(1U);
	MCG->SC &= ~MCG_SC_FCRDIV_MASK;
	SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(3U);
	MCG->C1 |= MCG_C1_IRCLKEN(1U);

	//Physical Connection (PORTA1 - UART0_RX, PORTA2 - UART0_TX)
	//PORT (pin? mux; UART? pull-up)
	PortConfigType UART_Config = {
			.mux = PORT_MUX_ALT2,
			.pull = PORT_PULL_UP,
	};
	PORT_Init(PORTA, 1, &UART_Config);
	PORT_Init(PORTA, 2, &UART_Config);

	//Baudrate(FCRDIV, SIM_SOPT2[UART0SRC])
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDL = 41U;
	uint32_t temp =UART0->C4;
	UART0->C4 = (UART0->C4 & ~UART0_C4_OSR_MASK) | UART0_C4_OSR(9U);

	//Frame (Data Length(C1[M], C4[M10]), Parity(C1[PE/PT], Stop bits(BDH[SBNS])
	UART0->C4 &= ~UART0_C4_M10_MASK;
	UART0->C4 |= UART0_C4_M10(0U);
	UART0->C1 &= ~UART0_C1_M_MASK;
	UART0->C1 |= UART0_C1_M(0U);
	UART0->C1 &= ~UART0_C1_PE_MASK;
	UART0->BDH &= UART0_BDH_SBNS_MASK;
}

void UART_SendChar(uint8_t character)
{
	//Enable Transmitter (TE)
	UART0->C2 |= UART_C2_TE(1U);
	//1. Wait TX Buffer Empty (TDRE)
	while ((UART0->S1 & UART_S1_TDRE_MASK) == 0U);
	//2. Write Data to TX Buffer (UART_D)
	UART0->D = character;
	//3. Wait Transmit Complete (TC = 1)
	while ((UART0->S1 & UART_S1_TC_MASK) == 0U);
	//Disable Transmitter (TE)
	UART0->C2 &= ~UART_C2_TE_MASK;
}

void UART_SendString(uint8_t *pData, uint8_t Len)
{
	uint8_t count;
	//Enable Transmitter (TE)
	UART0->C2 |= UART_C2_TE(1U);
	for (count = 0; count < Len; count++)
	{
		//1. Wait TX Buffer Empty (TDRE)
		while ((UART0->S1 & UART_S1_TDRE_MASK) == 0U);
		//2. Write Data to TX Buffer (UART_D)
		UART0->D = pData[count];
	}
	//3. Wait Transmit Complete (TC = 1)
	while ((UART0->S1 & UART_S1_TC_MASK) == 0U);
	//Disable Transmitter (TE)
	UART0->C2 &= ~UART_C2_TE_MASK;
}

uint8_t UART_ReceiveChar (void)
{
	//Enable Receiver
	UART0->C2 |= UART_C2_RE(1U);
	//Wait RX Data Buffer is Full (RDRF)
	while ((UART0->S1 & UART_S1_RDRF_MASK) == 0U);
	//Read RC Buffer
	uint8_t Data = UART0->D;
	//Disabla Receiver
	UART0->C2 &= ~UART_C2_RE_MASK;

	return Data;
}

uint8_t UART_ReceiveString (uint8_t Len)
{
	uint8_t count;
	uint8_t *pData;
	//Enable Receiver
	UART0->C2 |= UART_C2_RE(1U);
	for (count = 0; count < Len; count++)
	{
		//Wait RX Data Buffer is Full (RDRF)
		while ((UART0->S1 & UART_S1_RDRF_MASK) == 0U);
		//Read RC Buffer
		pData[count] = UART0->D;
	}
	//Disabla Receiver
	UART0->C2 &= ~UART_C2_RE_MASK;
	return pData;
}

void UART_ReceiveINT(void)
{
	// Enable Receive Interrupt
	UART0->C2 |= UART0_C2_RIE(1U);
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable Receiver
	UART0->C2 |= UART0_C2_RE(1U);
}


