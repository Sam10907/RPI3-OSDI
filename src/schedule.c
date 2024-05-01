#include "schedule.h"
#include "uart.h"
#include "timer.h"
#include "queue.h"
#include "mm.h"
#include "exception_table.h"

#define SPSR_EL1_MASK (0b0000 << 6)
#define SPSR_EL1_EL0 (0b0000 << 0)
#define SPSR_EL1_VALUE (SPSR_EL1_MASK | SPSR_EL1_EL0)

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
extern uint64_t remain_page;
extern struct page_t pages[PAGE_FRAMES_NUM];

void context_switch(struct task *next){
    struct task *prev = get_current(); 
    if (prev -> state == RUNNING) 
        task_queue_push(&run_queue,&run_queue_elmt[prev -> task_id]);
    update_current(next);
    update_pgd(next->mm.pgd);
    switch_to(&prev -> register_set, &next -> register_set);
}

void preempt_disable(){
    get_current() -> preempt_count = 0;
}

void preempt_enable(){
    get_current() -> preempt_count = 1;
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
                task_pool[i].resched = 0;
                task_pool[i].state = EXIT;
                task_pool[i].task_id = i;
                task_pool[i].time_slice = 5;
                task_pool[i].mm.pgd = 0;
                task_pool[i].preempt_count = 1;
                //free task i kernel stack page
                memzero((void*)(task_pool[i].mm.kernel_pages[0] | VIRT_BASE), PAGE_SIZE);
                uint32_t index = task_pool[i].mm.kernel_pages[0] >> 12;
                pages[index].status = AVAL;
                remain_page++;
                //clear task i register_set
                memzero(&task_pool[i].register_set,104);
            }
        }
        schedule();
    }
}

int privilege_task_create(void(*func)()){
    int i;
    for (i = 0; i < 64; i++)
    {
        if (task_pool[i].state == EXIT)
        {
            task_pool[i].state = RUNNING;
            uint64_t kernel_stack_page = allocate_kernel_page(&task_pool[i]);
            task_pool[i].register_set.x29 = kernel_stack_page | 0xfff;
            task_pool[i].register_set.sp = kernel_stack_page | 0xfff;
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
    init_task(&initial_task -> register_set);
}

void schedule(){
    struct task* next = task_queue_pop(&run_queue);
    context_switch(next);
}

int simple_loader(uint64_t start, uint64_t size, uint64_t pc){
    struct task* current_task = get_current();
    uint8_t *code_page = (uint8_t*) allocate_user_page(current_task,pc);
    allocate_user_page(current_task,USTACK_ADDR); // map user stack pointer(SP_EL0)
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

int copy_virtual_memory(struct task *dst){
    struct task *current = get_current();
    uint64_t code_va = allocate_user_page(dst,0x1000);
    uint64_t sp_va = allocate_user_page(dst,USTACK_ADDR);
    if (code_va == VIRT_BASE || sp_va == VIRT_BASE)
    {
        return -1;
    }
    memcpy((uint8_t*)(current -> mm.user_pages[0] | VIRT_BASE), (uint8_t*)code_va,PAGE_SIZE); //copy code page
    memcpy((uint8_t*)(current -> mm.user_pages[1] | VIRT_BASE), (uint8_t*)sp_va,PAGE_SIZE); // copy stack page
    return 0;
}

void copy_process(struct trapframe *tf){
    preempt_disable();
    int child = privilege_task_create(0);
    task_pool[child].register_set.sp -= 272;
    struct trapframe *child_trapframe = (struct trapframe*)task_pool[child].register_set.sp;
    memcpy((uint8_t*)tf, (uint8_t*)child_trapframe,272);
    task_pool[child].register_set.x30 = (uint64_t) ret_child_process; // lr register
    child_trapframe -> x[0] = 0;
    copy_virtual_memory(&task_pool[child]);
    preempt_enable();
    tf -> x[0] = child;
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
        task_pool[i].preempt_count = 1;
    }
}

void user_program(){
    extern uint64_t _binary_user_img_start;
    extern uint64_t _binary_user_img_end;
    uint64_t begin = (uint64_t)&_binary_user_img_start;
    uint64_t end = (uint64_t)&_binary_user_img_end;
    simple_loader(begin, end - begin+1, 0x1000);
}

void schedule_init(){
    task_queue_init(&run_queue);
    privilege_task_create(idle);
    privilege_task_create(zombie_reaper);
    privilege_task_create(user_program);
    init();
}