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

	// test command code in demo.c
	// test fork functionality
	/*int cnt = 0;
  	if(fork() == 0) {
		fork();
		fork();
		while(cnt < 10) {
			printf("task id: %d, sp: 0x%x cnt: %d\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
			delay(1000000);
		}
		return 0; // all childs exit
	}
	else
	{
		printf("task id: %d, sp: 0x%x cnt: %d\n", get_taskid(), &cnt, cnt++);
	}*/
	
	// test page fault handler
	/*if(fork() == 0) {
		int* a = 0x0; // a non-mapped address.
		printf("%d\n", *a); // trigger simple page fault, child will die here.
  	}*/
	return 0;
}
