#include "timer.h"

void core_timer_enable(){
    asm volatile ("mov x0, 1");
	asm volatile ("msr cntp_ctl_el0, x0"); // enable timer
	asm volatile ("mov x0, 0xfffffff");
	asm volatile ("msr cntp_tval_el0, x0"); // set expired time
	asm volatile ("mov x0, 2");
	asm volatile ("ldr x1, =0xffff000040000040");
	asm volatile ("str x0, [x1]"); // enable timer interrupt
}