#ifndef __MM_H_
#define __MM_H_

#define VIRT_BASE 0xFFFF000000000000
#define PGD_SHIFT 39
#define PUD_SHIFT 30
#define PMD_SHIFT 21
#define PTE_SHIFT 12
#define PAGE_SIZE 0x1000
#define DEVICE_BASE 0x3F000000
#define MAX_PHYSICAL_ADDR 0x40000000
#define PAGE_FRAMES_NUM         (0x40000000 >> 12)
#define USTACK_ADDR (0x0000ffffffffe000 - 8)

#ifndef __ASSEMBLY__

#include "typedef.h"
#include "schedule.h"
enum page_status {
    AVAL,
    USED,
};

struct page_t {
    enum page_status status;
};

void mm_init();
uint64_t get_free_page();
uint64_t allocate_user_page(struct task *task, uint64_t va);
void map_page(struct task *task, uint64_t va, uint64_t page);
uint64_t map_table(uint64_t *table, uint32_t shift, uint64_t va, int* new_table);
void map_table_entry(uint64_t *pte, uint64_t va, uint64_t pa);
void page_free(struct task *curr);
extern void memzero(void* start,  uint32_t size);
extern void memcpy(uint8_t* start,  uint8_t* end, uint64_t size);
#endif
#endif