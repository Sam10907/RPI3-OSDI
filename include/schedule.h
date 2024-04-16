#include "typedef.h"
#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__
#define MAX_PROCESS_PAGES 16
struct cpu_register
{
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29; // frame pointer
    uint64_t x30; // lr register
    uint64_t sp; // stack pointer
};

struct mm_struct
{
    uint64_t pgd;
    int user_pages_count;
	uint64_t user_pages[MAX_PROCESS_PAGES];
	int kernel_pages_count;
	uint64_t kernel_pages[MAX_PROCESS_PAGES];    
};

enum state{
    RUNNING,
    ZOMBIE,
    EXIT,
};

struct task
{
    uint64_t task_id;
    int time_slice;
    int resched;
    enum state state;
    struct cpu_register register_set;
    struct mm_struct mm;
};

struct trapframe
{
    uint64_t x[31];
    uint64_t sp_el0;
    uint64_t elr_el1;
    uint64_t spsr_el1;
};

void context_switch(struct task *next);
void privilege_task_create(void(*func)());
void init();
void exec_user_program(void (*func)());
void task_init();
void schedule();
int do_exec(uint64_t start, uint64_t size, uint64_t pc);
void do_exit(int);
void schedule_init();

#endif