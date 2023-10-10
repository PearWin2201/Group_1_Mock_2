/*
 * Copyright 2016-2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    BlinkLED1.c
 * @brief   Application entry point.
 */
#include "MKL46Z4.h"
#include "driver/driver.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */

volatile uint8_t a = 0;

void PIT_IRQHandler(void)
{
	// Check & Clear Interrupt
	if ((PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) != 0U)
	{
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
		a = 1;
	}
	// Toggle LED
	GPIOD->PTOR |= GPIO_PTOR_PTTO(1 << LED_GREEN_PIN);
}

void PIT_Delay500ms(void)
{
	PIT_StartTimer(0);
	while(a != 1);
	a = 0;
}

void main(void)
{
	Clock_LED_Init();
	Clock_PIT_Init();
	LED_Init();
	PIT_Init();
	NVIC_SetPriority(PIT_IRQn, 0U);
	PIT_StartTimer(0);
	while(1)
	{

	}
}
