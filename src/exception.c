#include "uart.h"
#include "context_switch.h"
#include "Sys.h"
#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define EXPIRE_PERIOD 0xfffff00

char irq_stack[4096];
extern struct task* get_current ();
extern struct task* get_user_current ();

/*void core_timer_enable(){
	asm volatile ("mov x0, 1");
	asm volatile ("msr cntp_ctl_el0, x0"); // enable timer
	asm volatile ("mov x0, 0xfffffff");
	asm volatile ("msr cntp_tval_el0, x0"); // set expired time
	asm volatile ("mov x0, 2");
	asm volatile ("ldr x1, =0x40000040");
	asm volatile ("str x0, [x1]"); // enable timer interrupt
}*/

void show_exception_message(unsigned long long elr, unsigned long long esr){
	int class = (esr >> 26) & 0b111111;
	int iss = esr & 0x1ffffff;
	if (class == 0b010101){ //system call
		if (iss == 1){
			uart_printf("Exception return address %x\n",elr);
			uart_printf("Exception class (EC) %x\n",class);
			uart_printf("Instruction specific syndrom (ISS) %x\n",iss);
		}
		else if (iss == 2){
			//core_timer_enable();
		}
	}else
	{
		uart_printf("Exception return address %x\n",elr);
		uart_printf("Exception class (EC) %x\n",class);
		uart_printf("Instruction specific syndrom (ISS) %x\n",iss);
	}
	
}

void core_timer_interrupt(){
	uart_printf("core timer interrupt\n");
	asm volatile ("mrs x0, cntfrq_el0");
	asm volatile ("msr cntp_tval_el0, x0");
}

void core_timer_ISR(){
	register unsigned int expired_period = EXPIRE_PERIOD;
	asm volatile("msr cntp_tval_el0, %0" : : "r"(expired_period));
	struct task *curr = get_current();
	if (--curr -> time_slice <= 0){
		curr -> resched = 1;
		curr -> time_slice = 0;
	}
	uart_printf("curr time slice %d\n",curr -> time_slice);
	asm volatile("msr daifclr, #2");
}

void core_timer_ISR_user(){
	register unsigned int expired_period = EXPIRE_PERIOD;
	asm volatile("msr cntp_tval_el0, %0" : : "r"(expired_period));
	struct task *curr = get_user_current();
	if (--curr -> time_slice <= 0){
		curr -> resched = 1;
		curr -> time_slice = 0;
	}
	uart_printf("curr time slice %d\n",curr -> time_slice);
	asm volatile("msr daifclr, #2");
}

void sys_get_taskid(struct trapframe *tf){
	struct task *curr = get_user_current();
	tf -> x[0] = curr -> task_id;
}

void sys_exit(struct trapframe *tf){
	do_exit(tf -> x[0]);
}

void irq_sp_switch(){
	register char *irq_sp;
	asm volatile("mov %0, sp" : "=r"(irq_sp)); // irq_sp is kernel sp
	if (!((irq_sp >= irq_stack) && (irq_sp <= &irq_stack[4095]))){
		asm volatile("mov sp, %0" : : "r"(&irq_stack[4095]));
	}
	core_timer_ISR();
	//restore kernel stack pointer
	if (!((irq_sp >= irq_stack) && (irq_sp <= &irq_stack[4095]))){
		asm volatile("mov sp, %0" : : "r"(irq_sp));
	}
}

void irq_sp_switch_user(){
	register char *irq_sp;
	asm volatile("mov %0, sp" : "=r"(irq_sp)); // irq_sp is kernel sp
	if (!((irq_sp >= irq_stack) && (irq_sp <= &irq_stack[4095]))){
		asm volatile("mov sp, %0" : : "r"(&irq_stack[4095]));
	}
	core_timer_ISR_user();
	//restore kernel stack pointer
	if (!((irq_sp >= irq_stack) && (irq_sp <= &irq_stack[4095]))){
		asm volatile("mov sp, %0" : : "r"(irq_sp));
	}
}

void irq_return(){
	struct task *curr = get_current();
	if (curr -> resched)
	{
		curr -> resched = 0;
		curr -> time_slice = 5;
		schedule();
	}
}

void irq_return_user(){
	struct task *curr = get_user_current();
	if (curr -> resched)
	{
		curr -> resched = 0;
		curr -> time_slice = 5;
		schedule();
	}
}

void sys_call_router(uint64_t sys_call_num, struct trapframe* tf) {
	switch (sys_call_num) {
        case SYS_GET_TASK_ID:
            sys_get_taskid(tf);
            break;
		case SYS_EXIT:
			sys_exit(tf);
			break;
	}
}

void syscall_table(unsigned long long elr, unsigned long long esr, struct trapframe *tf){
	int class = (esr >> 26) & 0b111111;
	if (class == 0b010101){ //system call
		uint64_t syscall_num = tf->x[8];
        sys_call_router(syscall_num, tf);
	}
}