#include "uart.h"
#include "message.h"

int main(void){
	uart_init();
	handle_message();
}
