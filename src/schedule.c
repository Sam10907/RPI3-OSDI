#include "schedule.h"
#include "uart.h"
#include "timer.h"
#include "queue.h"
#include "demo.h"
#include "mm.h"

extern void update_current(struct task*);
extern void update_user_current(struct task*);
extern void init_task (struct cpu_register*);
extern struct task* get_current ();
extern struct task* get_user_current ();
extern void switch_to (struct cpu_register*, struct cpu_register*);
extern void update_pgd(uint64_t pgd);
struct task_queue run_queue;
struct task_queue_elmt run_queue_elmt[64];
struct task task_pool[64];
char kernel_task_stack[64][4096];
char user_task_stack[64][4096];
extern uint64_t remain_page;

void context_switch(struct task *next){
    struct task *prev = get_current(); 
    if (prev -> state == RUNNING) 
        task_queue_push(&run_queue,&run_queue_elmt[prev -> task_id]);
    update_current(next);
    update_pgd(next->mm.pgd);
    switch_to(&prev -> register_set, &next -> register_set);
}

void idle(){
    uart_printf("enter idle\n");
    uart_printf("Remain pages: %d\n",remain_page);
    while(1){}
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

void init(){
    struct task* initial_task = task_queue_pop(&run_queue);
    update_current(initial_task);
    init_task(&initial_task -> register_set);
}

void schedule(){
    struct task* next = task_queue_pop(&run_queue);
    context_switch(next);
}

int do_exec(uint64_t start, uint64_t size, uint64_t pc){
    struct task* current_task = get_current();
    uint8_t *code_page = (uint8_t*) allocate_user_page(current_task,pc);
    uint8_t *stack_page = (uint8_t*) allocate_user_page(current_task,USTACK_ADDR);
    memcpy((uint8_t*)start, code_page, size);

    asm volatile("msr sp_el0, %0" : : "r"(USTACK_ADDR));
    asm volatile("msr elr_el1, %0": : "r"(pc));
    asm volatile("msr spsr_el1, %0" : : "r"(SPSR_EL1_VALUE));

    update_pgd(current_task->mm.pgd);

    asm volatile("eret");

    return 0;
}

void do_exit(int status){
    struct task *curr_kernel = get_current(); 
    if (status == 0)
    {
        curr_kernel -> state = ZOMBIE;
        page_free(curr_kernel);
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
        task_pool[i].mm.pgd = 0;
        task_pool[i].mm.kernel_pages_count = 0;
        task_pool[i].mm.user_pages_count = 0;
    }
}

void user_program(){
    extern uint64_t _binary_user_img_start;
    extern uint64_t _binary_user_img_end;
    uint64_t begin = (uint64_t)&_binary_user_img_start;
    uint64_t end = (uint64_t)&_binary_user_img_end;
    do_exec(begin, end - begin+1, 0x1000);
}

void schedule_init(){
    task_queue_init(&run_queue);
    privilege_task_create(idle);
    privilege_task_create(zombie_reaper);
    privilege_task_create(user_program);
    init();
}