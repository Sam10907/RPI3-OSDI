#include "typedef.h"
#ifndef __CONTEXT_SWITCH_H__
#define __CONTEXT_SWITCH_H__
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
};

struct trapframe
{
    uint64_t x[31];
    uint64_t sp_el0;
    uint64_t elr_el1;
    uint64_t spsr_el1;
};

void schedule();
void do_exec();
void do_exit(int);

#endif