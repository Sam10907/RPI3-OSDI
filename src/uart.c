#include "reg.h"
#include "uart.h"
#include "my_string.h"
void uart_init(){
	//mini UART
	*AUXENB |= (0x1 << 0);
	*AUX_MU_CNTL_REG = 0;
	*AUX_MU_IER_REG = 0;
	*AUX_MU_LCR_REG = 3;
	*AUX_MU_MCR_REG = 0;
	*AUX_MU_BAUD = 270;
	*AUX_MU_IIR_REG = 6;
	//GPIO
	*GPFSEL1 &= ~((0x7 << 12) | (0x7 << 15)); // reset GPIO 14, 15
	*GPFSEL1 |= ((0x1 << 13) | (0x1 << 16));
	*GPPUD = 0;
	register unsigned int r = 150;
	while (r--) {
        	asm volatile("nop"); //wait 150 cycles
    	}
    	*GPPUDCLK0 = (0x1 << 14) | (0x1 << 15); //bug??
    	r = 150;
    	while (r--) {
        	asm volatile("nop"); //wait 150 cycles
    	}
    	*GPPUDCLK0 = 0;
    	//enable TX, RX
	*AUX_MU_CNTL_REG = 3;
}

char uart_read(){
	do{
		asm volatile("nop");
	}while(!(*AUX_MU_LSR_REG & 0x1));
	char data = (char)*AUX_MU_IO_REG;
	return data;
}

void uart_write(unsigned int c){
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR_REG & 0x20));
    *AUX_MU_IO_REG = c;
}

void uart_printf(char* fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    extern volatile unsigned char __kernel_end;  // defined in linker
    char* s = (char*)&__kernel_end;              // put temporary string after code
    vsprintf(s, fmt, args);

    while (*s) {
        if (*s == '\n') uart_write('\r');
        uart_write(*s++);
    }
}

