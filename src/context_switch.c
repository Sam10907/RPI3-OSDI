#include "context_switch.h"
#include "uart.h"
#include "timer.h"
#include "queue.h"
#include "demo.h"

extern void update_current(struct task*);
extern void update_user_current(struct task*);
extern void init_task (struct cpu_register*);
extern struct task* get_current ();
extern struct task* get_user_current ();
extern void switch_to (struct cpu_register*, struct cpu_register*);
struct task_queue run_queue;
struct task_queue_elmt run_queue_elmt[64];
struct task task_pool[64];
char kernel_task_stack[64][4096];
char user_task_stack[64][4096];

void context_switch(struct task *next){
    struct task *prev = get_current(); 
    if (prev -> state == RUNNING) 
        task_queue_push(&run_queue,&run_queue_elmt[prev -> task_id]);
    update_current(next);
    switch_to(&prev -> register_set, &next -> register_set);
}

void delay(int num){
    while (num--){}
}

void idle(){
    uart_printf("enter idle\n");
    while(1){
        //delay(100000000);
        //uart_printf("enter idle\n");
    }
}

void test_task0(){ // test case function
    while (1) {
        uart_printf("task0\n");
        delay(100000000);
    }
}

void test_task1(){ // test case function
    while (1) {
        uart_printf("task1\n");
        delay(100000000);
    }
}

void zombie_reaper() {
    uart_printf("enter zombie reaper\n");
    while (1) {
        for (int i = 0; i < 64; i++) {
            if (task_pool[i].state == ZOMBIE) {
                uart_printf("reaper %d!\n", i);
                task_pool[i].state = EXIT;
                // WARNING: release kernel stack if dynamic allocation
            }
        }
        //schedule();
    }
}

void privilege_task_create(void(*func)()){
    int i;
    for (i = 0; i < 64; i++)
    {
        if (task_pool[i].state == EXIT)
        {
            task_pool[i].state = RUNNING;
            task_pool[i].register_set.x29 = (uint64_t) &kernel_task_stack[i][4095];
            task_pool[i].register_set.sp = (uint64_t) &kernel_task_stack[i][4095];
            task_pool[i].register_set.x30 = (uint64_t) func;
            break;
        }
    }
    task_queue_elmt_init(&run_queue_elmt[i],&task_pool[i]);
    task_queue_push(&run_queue,&run_queue_elmt[i]);
}

int user_task_create(void(*func)()){
    int i;
    for (i = 0; i < 64; i++)
    {
        if (task_pool[i].state == EXIT)
        {
            task_pool[i].state = RUNNING;
            task_pool[i].register_set.x29 = (uint64_t) &user_task_stack[i][4095];
            task_pool[i].register_set.sp = (uint64_t) &user_task_stack[i][4095];
            task_pool[i].register_set.x30 = (uint64_t) func;
            break;
        }
    }
    task_queue_elmt_init(&run_queue_elmt[i],&task_pool[i]);
    task_queue_push(&run_queue,&run_queue_elmt[i]);
    return i;
}

void init(){
    struct task* initial_task = task_queue_pop(&run_queue);
    update_current(initial_task);
    core_timer_enable();
    init_task(&initial_task -> register_set);
}

void schedule(){
    struct task* next = task_queue_pop(&run_queue);
    context_switch(next);
}

void exec_user_program(void (*func)()){
    int task_id = user_task_create(func);
    update_user_current(&task_pool[task_id]);
    init_task(&task_pool[task_id].register_set);
}

void do_exec(){
    exec_user_program(example_code);
}

void do_exit(int status){
    struct task *curr = get_user_current();
    struct task *curr_kernel = get_current(); 
    if (status == 0)
    {
        curr -> state = ZOMBIE;
        curr_kernel -> state = ZOMBIE;
    }
    schedule();
}

void task_init(){
    for (int i = 0; i < 64; i++)
    {
        task_pool[i].resched = 0;
        task_pool[i].state = EXIT;
        task_pool[i].time_slice = 5;
        task_pool[i].task_id = i;
    }
}

/*void kernel_main(){ //test core timer context switch
    privilege_task_create(test_task0);
    privilege_task_create(test_task1);
    init();
}*/

void boot_main(){
    task_init();
    task_queue_init(&run_queue);
    privilege_task_create(idle);
    privilege_task_create(zombie_reaper);
    privilege_task_create(demo_syscall);
    init();
}