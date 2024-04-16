#include "mylib.h"
#include "shell.h"
enum shell_status{
	Read,
	Parse
};
int main(void){
	enum shell_status STATUS = Read;
	while(1){
		printf("#");
		if (STATUS == Read){
			input_shell();
			STATUS = Parse;
		}
		if (STATUS == Parse){
			int status = parse_cmdline();
			if (!status) break;
			STATUS = Read;
		}
	}
	return 0;
}
