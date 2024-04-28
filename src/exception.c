#include "uart.h"
#include "schedule.h"
#include "Sys.h"
#include "mm.h"
#define EXPIRE_PERIOD 0xfffff00

char irq_stack[4096];
extern struct task* get_current ();
extern struct task* get_user_current ();

void show_exception_message(unsigned long long elr, unsigned long long esr){
	int class = (esr >> 26) & 0b111111;
	int iss = esr & 0x1ffffff;
	if (class == 0b010101){ // test command: exc in mini shell
		if (iss == 1){ // exc: svc #1
			uart_printf("Exception return address %x\n",elr);
			uart_printf("Exception class (EC) %x\n",class);
			uart_printf("Instruction specific syndrom (ISS) %x\n",iss);
		}
	}
	else // other exception occur
	{
		uart_printf("Exception return address %x\n",elr);
		uart_printf("Exception class (EC) %x\n",class);
		uart_printf("Instruction specific syndrom (ISS) %x\n",iss);
	}
	
}

void core_timer_ISR(){
	register unsigned int expired_period = EXPIRE_PERIOD;
	asm volatile("msr cntp_tval_el0, %0" : : "r"(expired_period));
	struct task *curr = get_current();
	if (--curr -> time_slice <= 0){
		curr -> resched = 1;
		curr -> time_slice = 0;
	}
	//uart_printf("curr time slice %d\n",curr -> time_slice);
	asm volatile("msr daifclr, #2");
}

void sys_get_taskid(struct trapframe *tf){
	struct task *curr = get_current();
	tf -> x[0] = curr -> task_id;
}

void sys_exit(struct trapframe *tf){
	do_exit(tf -> x[0]);
}

void sys_uart_read(struct trapframe *tf){
	char *buf = (char*)tf -> x[0];
	char temp;
	int i = 0;
	while((temp = uart_read()) != '\r'){
		*(buf + i) = temp;
		uart_write(temp);
		i++;
	}
	*(buf + i) = '\0';
	tf -> x[0] = 1; //complete
}

void sys_uart_write(struct trapframe *tf){
	char *buf = (char*)tf -> x[0];
	while(*buf){
		uart_write(*buf);
		buf++;
	}
	tf -> x[0] = 1; //complete
}

void sys_remain_page_num(struct trapframe *tf){
	extern uint32_t remain_page;
	tf -> x[0] = remain_page;
}

void sys_fork(struct trapframe *tf){
	copy_process(tf);
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

void irq_return(){
	struct task *curr = get_current();
	if (curr -> resched && curr -> preempt_count)
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
		case SYS_UART_READ:
			sys_uart_read(tf);
			break;
		case SYS_UART_WRITE:
			sys_uart_write(tf);
			break;
		case SYS_REMAIN_PAGE_NUM:
			sys_remain_page_num(tf);
			break;
		case SYS_FORK:
			sys_fork(tf);
			break;
	}
}

void page_fault_handler() {
    register uint64_t fault_addr;
    asm volatile("mrs %0, FAR_EL1": "=r"(fault_addr));
    uart_printf("Page fault address at 0x%x, killed\n", fault_addr);
	do_exit(0);
}

void syscall_table(unsigned long long elr, unsigned long long esr, struct trapframe *tf){
	int class = (esr >> 26) & 0b111111;
	int iss = esr & 0x1ffffff;
	if (class == 0b010101){ //system call
		if (iss == 1)
		{
			show_exception_message(elr,esr);
		}
		else
		{
			uint64_t syscall_num = tf->x[8];
        	sys_call_router(syscall_num, tf);
		}
	}
	else if(class == 0x24 ){
		page_fault_handler();
	}
}