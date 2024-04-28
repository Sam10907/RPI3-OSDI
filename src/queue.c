#include "queue.h"
#include "uart.h"

void task_queue_init(struct task_queue* q){
    q -> front = 0;
    q -> rear = 0;
}

void task_queue_elmt_init(struct task_queue_elmt* elmt, struct task *task){
    elmt -> task = task;
    elmt -> next = 0;
}

void task_queue_push(struct task_queue* q, struct task_queue_elmt* elmt){
    if ((q -> front == 0) && (q -> rear == 0))
    {
        q -> front = elmt;
        q -> rear = elmt;
    }
    else{
        q -> rear -> next = elmt;
        q -> rear = elmt;
    }
}

struct task* task_queue_pop(struct task_queue* q){
    if ((q -> front == 0) && (q -> rear == 0)) return 0;
    struct task_queue_elmt* poped_elmt = q -> front;
    q -> front = q -> front -> next;
    if (q -> front == 0) q -> rear = 0;
    poped_elmt -> next = 0;
    return poped_elmt -> task;
}

void print_queue_info(struct task_queue* q){
    struct task_queue_elmt *curr = q -> front;
    while(curr){
       uart_printf("task id: %d\n",curr -> task -> task_id);
        curr = curr -> next;
    }
}