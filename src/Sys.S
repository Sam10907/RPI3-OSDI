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