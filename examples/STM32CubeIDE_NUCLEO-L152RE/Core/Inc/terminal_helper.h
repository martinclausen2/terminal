/*
 * terminal_helper.h
 *
 *  Created on: 30.12.2022
 *      Author: Martin
 */

#ifndef INC_TERMINAL_HELPER_H_
#define INC_TERMINAL_HELPER_H_

#include "../terminal/terminal.h"
#include "../terminal/terminal_config.h"
#include "../terminal/commands.h"

extern UART_HandleTypeDef huart2;

void _reset_fcn();

void TUSART_PutChar(char c);

void TUSART_Print(const char* str);

#endif /* INC_TERMINAL_HELPER_H_ */
