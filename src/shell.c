#include "shell.h"
#include "uart.h"
#include "my_string.h"
static char cmd[128] = "";

void init_shell(){
	uart_init();
}

void input_shell(){
	char temp;
	char *ptr = cmd;
	while((temp = uart_read()) != '\r'){
		uart_write(temp);
		*ptr = temp;
		ptr++;
	}
	*ptr = '\0';
	uart_write('\n');
}

void parse_cmdline(){
	if (!strcmp(cmd,"exc")){
		asm volatile ("svc #1");
	}
	else if (!strcmp(cmd,"irq")){
		asm volatile ("svc #2");
	}
	else{
		uart_printf("%s\n",cmd);
	}
	//flush cmd buffer
	int i = 0;
	for ( ; cmd[i] != '\0' ; i++) cmd[i] = '\0';
}
