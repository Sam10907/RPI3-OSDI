#include "shell.h"
#include "mylib.h"
#include "typedef.h"
static char cmd[128] = "";

void input_shell(){
	read_from_uart(cmd);
	char end[2] = {'\n','\0'};
	write_to_uart(end);
}

int parse_cmdline(){
	if (!strcmp(cmd,"exc")){
		asm volatile ("svc #1");
	}
	else if (!strcmp(cmd,"demo")){
		uint64_t task_id = get_taskid();
    	printf("This task id is %d\n",task_id); 
	}
	else if (!strcmp(cmd,"exit")){
		return 0;	 
	}
	else if (!strcmp(cmd,"free")){
		int free_pages = remain_page_num();
		printf("Free pages num: %d\n",free_pages);
	}
	else{
		printf("%s\n",cmd);
	}
	//flush cmd buffer
	int i = 0;
	for ( ; cmd[i] != '\0' ; i++) cmd[i] = '\0';
	return 1;
}
