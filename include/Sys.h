#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_TASK_ID     0
#define SYS_EXIT 1
#define SYS_UART_READ 2
#define SYS_UART_WRITE 3
#define SYS_REMAIN_PAGE_NUM 4

#endif

#ifndef __ASSEMBLY__

extern int get_taskid();
extern int exit(int);
extern int read_from_uart(char*);
extern int write_to_uart(const char*);
extern int remain_page_num();

#endif