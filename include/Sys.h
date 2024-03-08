#ifndef __SYS_H__
#define __SYS_H__

#define SYS_GET_TASK_ID     0
#define SYS_EXIT 1

#endif

#ifndef __ASSEMBLY__

extern int get_taskid();
extern int exit(int);

#endif