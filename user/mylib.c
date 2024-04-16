#include "mylib.h"
void printf(char *fmt, ...){
	__builtin_va_list args;
	__builtin_va_start(args, fmt);
	char buf[64];          
	vsprintf(buf, fmt, args);
	char *s = buf;
	write_to_uart(s);
}

int read(char *str){
	return read_from_uart(str);
}

int write(const char *str){
	return write_to_uart(str);
}