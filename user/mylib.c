#include "mylib.h"
void printf(char *fmt, ...){
	__builtin_va_list args;
	__builtin_va_start(args, fmt);
	char buf[64];          
	vsprintf(buf, fmt, args);
	char *s = buf;
	write_to_uart(s);
}
