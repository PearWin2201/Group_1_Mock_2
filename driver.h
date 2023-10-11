/*
 * driver.h
 *
 *  Created on: Oct 11, 2023
 *      Author: ADMIN
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include "MKL46Z4.h"

#define NULL ((void *)0)

#define LED_GREEN_PIN 		5U
#define LED_GREEN_PORT		PORTD
#define LED_RED_PIN			29U
#define LED_RED_PORT		PORTE

#define FCMD_READ         0X03U
#define FCMD_WRITE_WORD   0X06U
#define FCMD_ERASE_SECTOR 0X09U
#define GET_SECTOR_NUMBER(ADDR)   ((uint32_t)(ADDR)/1024U)

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

typedef void (*CallbackType)(uint8_t pinnum);

typedef struct {
	PortMuxType 		mux;
	PortPullType 		pull;
	PortInterruptType 	interrupt;
	CallbackType 		callback;
} PortConfigType;


#endif /* DRIVER_H_ */
