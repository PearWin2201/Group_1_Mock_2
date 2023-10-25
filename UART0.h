/*
 * UART0.h
 *
 *  Created on: Oct 19, 2023
 *      Author: DELL
 */

#ifndef UART0_H_
#define UART0_H_

void UART0_Init();
void UART_SendChar(uint8_t character);
void UART_SendString(uint8_t * pData, uint8_t Len);
uint8_t UART_ReceiveChar(void);
void UART_ReceiveINT(void);

#endif /* UART0_H_ */
