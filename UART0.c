/*
 * UART0.c
 *
 *  Created on: Oct 19, 2023
 *      Author: DELL
 */
#include "MKL46Z4.h"
#include "Port.h"
#include"UART0.h"

volatile uint8_t a = 0;
volatile uint8_t Data;
volatile uint8_t flag = 0;

void UART0_Init()
{
//    - Clock (Enable Clock PORTA / UART0 / 4MHz)
	SIM->SCGC4 |= SIM_SCGC4_UART0(1U);
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1U);
	MCG->C2 |= MCG_C2_IRCS(1U);
	MCG->SC &= ~MCG_SC_FCRDIV_MASK;
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(3U);
	MCG->C1 |= MCG_C1_IRCLKEN(1U);

//    - PORT (pin? mux = uart? pull-up)(PA1 - UART0_RX / PA2 - UART0_TX)
	const PortConfigType UART0_Config = {
			.mux = PORT_MUX_ALT2,
			.pull = PORT_PULL_UP
	};
	PORT_Init(PORTA, 1, &UART0_Config);
	PORT_Init(PORTA, 2, &UART0_Config);

//    - Baudrate (Clock - Async module clock / Baud Divider / OSR Divider)
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDL = 41U;

	uint32_t temp = UART0->C4;	// Read
	temp &= ~UART0_C4_OSR_MASK;	// Modify
	temp |= UART0_C4_OSR(9U);
	UART0->C4 = temp;			// Write

//	UART0->C4 = (UART0->C4 & ~UART0_C4_OSR_MASK) | UART0_C4_OSR(9U);

//    - Frame
//        - Data Length (8) C1[M] & C4[M10]
	UART0->C4 &= ~UART0_C4_M10_MASK;
	UART0->C1 &= ~UART0_C1_M_MASK;

//        - Parity (None) C1[PE/PT]
	UART0->C1 &= ~UART0_C1_PE_MASK;

//        - Stop bits (1) BDH[SBNS])
	UART0->BDH &= ~UART0_BDH_SBNS_MASK;
}

void UART_SendChar(uint8_t character)
{
	// Enable Transmitter
	UART0->C2 |= UART0_C2_TE(1U);

	// 1. Wait TX Buffer is Empty (TDRE = 1)
	while ( (UART0->S1 & UART0_S1_TDRE_MASK) == 0U );

	// 2. Write Data to TX Buffer
	UART0->D = character;

	// 3. Wait Transmit is complete (TC = 1)
	while ( (UART0->S1 & UART0_S1_TC_MASK) == 0U );

	// Disable Transmitter
	UART0->C2 &= ~UART0_C2_TE_MASK;
}

void UART_SendString(uint8_t * pData, uint8_t Len)
{
	uint8_t count;

	// Enable Transmitter
	UART0->C2 |= UART0_C2_TE(1U);

	for (count = 0; count < Len; count++)
	{
		// 1. Wait TX Buffer is Empty (TDRE = 1)
		while ( (UART0->S1 & UART0_S1_TDRE_MASK) == 0U );

		// 2. Write Data to TX Buffer
		UART0->D = pData[count];
	}

	// 3. Wait Transmit is complete (TC = 1)
	while ( (UART0->S1 & UART0_S1_TC_MASK) == 0U );

	// Disable Transmitter
	UART0->C2 &= ~UART0_C2_TE_MASK;
}

uint8_t UART_ReceiveChar(void)
{
	uint8_t Data;
	// Enable Receiver
	UART0->C2 |= UART0_C2_RE(1U);

	// Wait RX Data Buffer is Full (RDRF = 1)
	while ( (UART0->S1 & UART0_S1_RDRF_MASK) == 0U );

	// Read RX Buffer
	Data = UART0->D;

	// Disable Receiver
	UART0->C2 &= ~UART0_C2_RE_MASK;

	return Data;
}

void UART_ReceiveINT(void)
{
	// Enable Receive Interrupt
	UART0->C2 |= UART0_C2_RIE(1U);
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable Receiver
	UART0->C2 |= UART0_C2_RE(1U);
}





