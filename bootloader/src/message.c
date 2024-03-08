#include "uart.h"
void handle_message(){
	// image size
	int imageSize = 0, i = 0;
	for (; i < 4 ; i++){
		imageSize <<= 8;
		imageSize = imageSize | (int)uart_read();
	}
	uart_printf("Image Size: %d\n",imageSize);
	//checksum
	char checksum[4] = "";
	i = 3;
	for ( ; i >=0 ; i--){
		checksum[i] = uart_read();
	}
	uart_printf("Checksum: %d\n",*(int*)checksum);
	//receive kernel8.img
	char *kernel = (char*)0x80000;
	char byte;
	int sum = 0;
	for (i = 0 ; i < imageSize ; i++){
		//uart_printf("%d\n",sum);
		byte = uart_read();
		*(kernel+i) = byte;
		sum += (int)byte;
	}
	if (sum != *(int*)checksum){
		uart_printf("Start os failed!\n");
	}
	else{
		void (*start_os)(void) = (void *)kernel;
		uart_printf("Bootloader is loading rpi3 kernel...\n");
		start_os();
	}
}
