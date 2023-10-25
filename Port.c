/*
 * Port.c
 *
 *  Created on: Oct 19, 2023
 *      Author: DELL
 */
#include "Port.h"

void PORT_Init(PORT_Type * PORTx, PinType pinnum, const PortConfigType * Config)
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

	}


}

