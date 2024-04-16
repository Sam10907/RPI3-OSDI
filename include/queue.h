#include "schedule.h"
#ifndef __QUEUE_H__
#define __QUEUE_H__
struct task_queue_elmt {  
    struct task* task;
    struct task_queue_elmt* next;
};

struct task_queue {
    struct task_queue_elmt* front;
    struct task_queue_elmt* rear;
};

void task_queue_init(struct task_queue* q);
void task_queue_elmt_init(struct task_queue_elmt* elmt, struct task *task);
void task_queue_push(struct task_queue* q, struct task_queue_elmt* elmt);
struct task* task_queue_pop(struct task_queue* q);

#endif