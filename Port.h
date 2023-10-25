/*
 * Port.h
 *
 *  Created on: Oct 19, 2023
 *      Author: phiphi
 */



#ifndef PORT_H_
#define PORT_H_

#include "MKL46Z4.h"

#define NULL ((void *)0)

typedef enum {
	PORT_MUX_ANALOG		= 0x0U,
	PORT_MUX_GPIO		= 0x1U,
	PORT_MUX_ALT2,
	PORT_MUX_ALT3,
	PORT_MUX_ALT4,
	PORT_MUX_ALT5,
	PORT_MUX_ALT6,
	PORT_MUX_ALT7,
} PortMuxType;

typedef enum {
	PORT_PULL_DISABLE	= 0x0U,
	PORT_PULL_DOWN		= 0x2U,
	PORT_PULL_UP		= 0x3U,
} PortPullType;

typedef enum {
	PORT_INTERRUPT_DISABLE		= 0x0U,
	PORT_INTERRUPT_LOW			= 0x8U,
	PORT_INTERRUPT_RISING		= 0x9U,
	PORT_INTERRUPT_FALLING		= 0xAU,
	PORT_INTERRUPT_EITHER		= 0xBU,
	PORT_INTERRUPT_HIGH			= 0xCU,
} PortInterruptType;

typedef enum {
	pin0     = 0u,
	pin1     = 1u,
	pin2,
	pin3,
	pin4,
	pin5,
	pin6,
	pin7,
	pin8,
	pin9,
	pin10,
	pin11,
	pin12,
	pin13,
	pin14,
	pin15,
	pin16,
	pin17,
	pin18,
	pin19,
	pin20,
	pin21,
	pin22,
	pin23,
	pin24,
	pin25,
	pin26,
	pin27,
	pin28,
	pin29,
	pin30,
	pin31,
}PinType;

typedef void (*CallbackType)(uint8_t pinnum);

typedef struct {
	PortMuxType 		mux;
	PortPullType 		pull;
	PortInterruptType 	interrupt;
	CallbackType 		callback;
} PortConfigType;


void PORT_Init(PORT_Type * PORTx, PinType pinnum, const PortConfigType * Config);

#endif /* PORT_H_ */
