#define __ASSEMBLY__
#include "Sys.h"

.global get_taskid
get_taskid:
    mov x8, SYS_GET_TASK_ID
    svc #0
    ret

.global exit
exit:
    mov x8, SYS_EXIT
    svc #0

.global read_from_uart
read_from_uart:
    mov x8, SYS_UART_READ
    svc #0
    ret

.global write_to_uart
write_to_uart:
    mov x8, SYS_UART_WRITE
    svc #0
    ret

.global remain_page_num
remain_page_num:
    mov x8, SYS_REMAIN_PAGE_NUM
    svc #0
    ret