/*
 * terminal_helper.c
 *
 *  Created on: 30.12.2022
 *      Author: Martin
 */

#include "terminal_helper.h"

char dbgbuffer[256];

void _reset_fcn()
{
	NVIC_SystemReset();
}

inline void TUSART_PutChar(char c)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, 0xFFFF);
}

void TUSART_Print(const char* str)
{
	unsigned int length = strlen(str);
	HAL_UART_Transmit(&huart2, (uint8_t *)str, length, 0xFFFF);
}
