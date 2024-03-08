#include "uart.h"
#include "shell.h"
enum shell_status{
	Read,
	Parse
};
int main(void){
	enum shell_status STATUS = Read;
	init_shell();
	while(1){
		uart_write('#');
		if (STATUS == Read){
			input_shell();
			STATUS = Parse;
		}
		if (STATUS == Parse){
			parse_cmdline();
			STATUS = Read;
		}
	}
}
