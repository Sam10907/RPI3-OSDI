#ifndef __UART_H_
#define __UART_H_
#include "reg.h"
#include "my_string.h"
void uart_init();
char uart_read();
void uart_write(unsigned int);
void uart_printf(char* fmt, ...);
#endif
