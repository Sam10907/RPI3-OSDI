#include "mm.h"
#include "mmu.h"
#include "schedule.h"
#include "uart.h"

struct page_t pages[PAGE_FRAMES_NUM];
uint32_t first_aval_page, last_aval_page;
uint32_t remain_page = 0;

void mm_init(){
    extern uint8_t __kernel_end;
    uint8_t* kernel_end = &__kernel_end;
    uint32_t last_used_page = (uint64_t)kernel_end >> 12;
    uint32_t mmio_start_page = DEVICE_BASE >> 12;
    uint32_t i = 0;
    for (; i <= last_used_page; i++)
    {
        pages[i].status = USED;
    }

    for (; i < mmio_start_page; i++)
    {
        remain_page++;
        pages[i].status = AVAL;
    }
    
    for (; i < PAGE_FRAMES_NUM; i++)
    {
        pages[i].status = USED;
    }
    first_aval_page = last_used_page + 1;
    last_aval_page = mmio_start_page - 1;
}

uint64_t get_free_page(){
    uint32_t i = first_aval_page;
    for (; i <= last_aval_page; i++)
    {
        if (pages[i].status == AVAL){
            pages[i].status = USED;
            remain_page--;
            uint64_t virt_addr = (i << 12) | VIRT_BASE;
            uint64_t phy_addr = i << 12;
            memzero((void*)virt_addr, PAGE_SIZE);
            return phy_addr;
        }
    }
    return 0;
}

uint64_t map_table(uint64_t *table, uint32_t shift, uint64_t va, int* new_table){
    uint32_t index = va >> shift;
    index = index & (0x200 - 1);
    if (!table[index]){
        *new_table = 1;
        uint64_t next_level_table = get_free_page();
        uint64_t entry = next_level_table | PD_TABLE;
        table[index] = entry;
        return next_level_table;
    } else {
        *new_table = 0;
    }
    return table[index] & 0xfffffffffffff000;
}

void map_table_entry(uint64_t *pte, uint64_t va, uint64_t pa){
    uint32_t index = va >> PTE_SHIFT;
    index = index & (0x200 - 1);
    uint64_t entry = pa | USER_PTE_NORMAL_ATTR;
    pte[index] = entry;
}

void map_page(struct task *task, uint64_t va, uint64_t page){
    uint64_t pgd;
    if (!task->mm.pgd) {
        task->mm.pgd = get_free_page();
        task->mm.kernel_pages[task->mm.kernel_pages_count++] = task->mm.pgd;
    }
    pgd = task->mm.pgd;
    int new_table;
    uint64_t pud = map_table((uint64_t *)(pgd | VIRT_BASE), PGD_SHIFT, va, &new_table);
    if (new_table) {
        task->mm.kernel_pages[task->mm.kernel_pages_count++] = pud;
    }
    uint64_t pmd = map_table((uint64_t *)(pud | VIRT_BASE) , PUD_SHIFT, va, &new_table);
    if (new_table) {
        task->mm.kernel_pages[task->mm.kernel_pages_count++] = pmd;
    }
    uint64_t pte = map_table((uint64_t *)(pmd | VIRT_BASE), PMD_SHIFT, va, &new_table);
    if (new_table) {
        task->mm.kernel_pages[task->mm.kernel_pages_count++] = pte;
    }
    map_table_entry((uint64_t *)(pte | VIRT_BASE), va, page);
    task->mm.user_pages[task->mm.user_pages_count++] = page;
}

uint64_t allocate_user_page(struct task *task, uint64_t va){
    uint64_t page = get_free_page();
    if (page == 0) {
        return 0;
    }
    map_page(task, va, page);
    return page | VIRT_BASE;
}

void page_free(struct task *curr){
    while (curr -> mm.kernel_pages_count--)
    {
        int count = curr -> mm.kernel_pages_count;
        uint32_t index = curr -> mm.kernel_pages[count] >> 12;
        pages[index].status = AVAL;
        remain_page++;
        memzero((void*)(curr -> mm.kernel_pages[count] | VIRT_BASE),PAGE_SIZE);
    }
    curr -> mm.kernel_pages_count = 0;
    while (curr -> mm.user_pages_count--)
    {
        int count = curr -> mm.user_pages_count;
        uint32_t index = curr -> mm.user_pages[count] >> 12;
        pages[index].status = AVAL;
        remain_page++;
        memzero((void*)(curr -> mm.user_pages[count] | VIRT_BASE),PAGE_SIZE);
    }
    curr -> mm.user_pages_count = 0;
}