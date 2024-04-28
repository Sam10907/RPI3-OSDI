#ifndef _EXCEPTION_TABLE_H
#define _EXCEPTION_TABLE_H

#define BAD_SYNC        0
#define BAD_IRQ         1
#define BAD_FIQ         2
#define BAD_ERROR       3

#ifndef  __ASSEMBLY__

extern void ret_child_process();

#endif
#endif