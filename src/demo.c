#include "schedule.h"
#include "uart.h"
#include "demo.h"
#include "Sys.h"
#include "typedef.h"

void example_code(){
    uint64_t task_id = get_taskid();
    uart_printf("This task id is %d\n",task_id); 
    exit(0);
}

/*void demo_syscall(){
    asm volatile("msr sp_el0, %0" : : "r"(SP_ADDR));
    asm volatile("msr spsr_el1, %0" : : "r"(SPSR_EL1_VALUE));
    asm volatile("msr elr_el1, %0": : "r"(do_exec));
    asm volatile("eret");
    do_exec();
}*/