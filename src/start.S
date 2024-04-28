#define __ASSEMBLY__
#include "mmu.h"
#include "mm.h"

#define CPACR_EL1_FPEN      (0b11 << 20)
#define CPACR_EL1_VALUE     (CPACR_EL1_FPEN)
//unmask el1 interrupt
#define SPSR_EL2_MASK_ALL   (0b0000 << 6)
#define SPSR_EL2_EL1h       (0b0101 << 0)
#define SPSR_EL2_VALUE      (SPSR_EL2_MASK_ALL | SPSR_EL2_EL1h)
//unmask el0 interrupt
#define SPSR_EL1_MASK       (0b0000 << 6)
#define SPSR_EL1_EL0        (0b0000 << 0)
#define SPSR_EL1_VALUE      (SPSR_EL1_MASK | SPSR_EL1_EL0)

.section ".text.boot"

.global _start
_start:
	// get cpu id
	mrs     x1, MPIDR_EL1
	and     x1, x1, #3
	cbz     x1, 2f
	// if cpu_id > 0, stop
1:
	wfe
	b       1b
	// if cpu_id == 0
2:
	/* The execution state for EL1 is AArch64 */
	mov x0, (1 << 31)
	msr hcr_el2, x0

	/* disabled mmu */
	ldr x1, =SCTLR_VALUE_MMU_DISABLED
	msr sctlr_el1, x1

	/* make el0, el1 can use Floating point and Advanced SIMD */
	ldr x1, =CPACR_EL1_VALUE
	msr CPACR_EL1, x1
	    
	/* mask all interrupt, and set interrupt level to el1h */
	ldr     x0, =SPSR_EL2_VALUE
	msr spsr_el2, x0

	/*set return function*/
	adr x0, el1_start
	msr elr_el2, x0
	eret

el1_start:
	ldr x1, =0x60000
	mov sp, x1
	/* set exception table */
	ldr x0, =exception_vector_table
	msr VBAR_EL1, x0
	/* mask el0 cpu interrupt */
	//mov x0, 0x3c0 
	//msr spsr_el1, x0
	ldr x0, =SPSR_EL1_VALUE
	msr spsr_el1, x0

	// setup tcr
    ldr     x0, =TCR_CONFIG_DEFAULT
    msr     tcr_el1, x0

    // setup mair
    ldr     x0, =MAIR_VALUE
    msr     mair_el1, x0

    // create page table
    bl      page_table_create

	adrp x0, pg_dir
	msr ttbr0_el1, x0  /* load PGD to the buttom translation based register. */
	msr ttbr1_el1, x0  /* load PGD to the upper translation based register. */

	 // clear bss
    adr x1, bss_begin  //pc-relative address
    adr x2, bss_end
	sub x2, x2, x1
	lsr x2, x2, #3
3: 
	cbz x2, 4f
	str xzr, [x1], #8
	sub x2, x2, #1
	cbnz x2, 3b

4:
	// mov sp to virtual address
	ldr x1, =VIRT_BASE
	add sp, sp, x1

	//core_timer_enable
	mov x0, 1
	msr cntp_ctl_el0, x0
	mov x0, 0xfffffff
	msr cntp_tval_el0, x0
	mov x0, 2
	ldr x1, =0x40000040
	str x0, [x1]

	ldr x0, =boot_main
    // enable MMU
    ldr x1, =SCTLR_VALUE_MMU_ENABLE
    mrs x2, sctlr_el1
    orr x2, x2, x1
    msr sctlr_el1, x2

	// indirect branch
	br x0
1:
	b 1b

	.macro create_pgd_entry, table, virt, tmp1, tmp2, tmp3
	create_table_entry \table, \virt, PGD_SHIFT, \tmp1, \tmp2
	create_table_entry \table, \virt, PUD_SHIFT, \tmp1, \tmp2
	create_pmd_table_entry \table, \virt, PMD_SHIFT, \tmp1, \tmp2, \tmp3
	.endm

	.macro create_table_entry, table, virt, shift, tmp1, tmp2
	lsr \tmp1, \virt, #\shift
	and \tmp1, \tmp1, #(0x200 - 1)  // table index
	add \tmp2, \table, #PAGE_SIZE
	orr \tmp2, \tmp2, #PD_TABLE
	str \tmp2, [\table, \tmp1, lsl #3]
	add \table, \table, #PAGE_SIZE 	 // next level page table (PGD -> PUD)
	.endm

	.macro create_pmd_table_entry, table, virt, shift, tmp1, tmp2, tmp3
	lsr \tmp1, \virt, #\shift
	and \tmp1, \tmp1, #(0x200 - 1)  // table index
	mov \tmp3, \table
loop:	add \tmp2, \table, #PAGE_SIZE
	orr \tmp2, \tmp2, #PD_TABLE
	str \tmp2, [\tmp3, \tmp1, lsl #3]
	add \table, \table, #PAGE_SIZE
	add \tmp1, \tmp1, #1
	cmp \tmp1, #511
	b.ls loop
	.endm

	.macro create_page_map, table, phy, start, end, mmu_flag, tmp
	lsr \start, \start, #PTE_SHIFT
	and \start, \start, #((1 << 18) - 1)
	lsr \end, \end, #PTE_SHIFT
	and \end, \end, #((1 << 18) - 1)
	lsr \phy, \phy, #PTE_SHIFT
	mov \tmp, #\mmu_flag
	orr \phy, \tmp, \phy, lsl #PTE_SHIFT
9999:	str \phy, [\table, \start, lsl #3]
	add \start, \start, #1
	add \phy, \phy, #PAGE_SIZE
	cmp \start, \end
	b.ls 9999b
	.endm

page_table_create:
	mov x29, x30 //save return address
	mov x1, VIRT_BASE
	adrp x0, pg_dir
	create_pgd_entry x0, x1, x2, x3, x4

	/* mapping kernel */
	mov x1, #0x0
	mov x2, #VIRT_BASE
	ldr x3, =((VIRT_BASE | DEVICE_BASE) - PAGE_SIZE)
	add x0, x4, #PAGE_SIZE
	create_page_map x0, x1, x2, x3, BOOT_PTE_NORMAL_ATTR, x4

	/* mapping mmio-device */
	mov x1, #DEVICE_BASE
	ldr x2, =(VIRT_BASE | DEVICE_BASE)
	ldr x3, =((VIRT_BASE | MAX_PHYSICAL_ADDR) - PAGE_SIZE)
	create_page_map x0, x1, x2, x3, BOOT_PTE_DEVICE_ATTR, x4

	mov x30, x29
	ret
