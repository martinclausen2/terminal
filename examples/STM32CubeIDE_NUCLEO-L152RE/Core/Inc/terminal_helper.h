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

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Size of Reception buffer */
#define RX_BUFFER_SIZE   256
/* Size of Transmission buffer */
#define TX_BUFFER_SIZE   256

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

void _reset_fcn();

void TUSART_PutChar(char c);

void TUSART_Print(const char* str);

void TUSART_StartReception(void);

void TUSART_UARTEx_RxEventCallback(uint16_t Size);

#endif /* INC_TERMINAL_HELPER_H_ */
