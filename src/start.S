.section ".text.boot"

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
	// set stack pointer
	ldr     x1, =_start
	mov     sp, x1

	// clear bss
	ldr     x1, =__bss_start
	ldr     x2, =__bss_size
3:  
	cbz     x2, 4f
	str     xzr, [x1], #8
	sub     x2, x2, #1
	cbnz    x2, 3b

4:
	/* The execution state for EL1 is AArch64 */
	mov x0, (1 << 31)
	msr hcr_el2, x0

	/* make el0, el1 can use Floating point and Advanced SIMD */
	ldr x1, =CPACR_EL1_VALUE
	msr CPACR_EL1, x1
	    
	//mov x0, 0x3c5  // EL1h (SPSel = 1) with interrupt disabled
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
	/*ldr x0, =SPSR_EL1_VALUE
	msr spsr_el1, x0*/
	bl boot_main //test context switch
	/*adr x0, el0_start
	msr elr_el1, x0
	eret*/
el0_start:
	ldr x1, =0x40000
	mov sp, x1
	bl main //mini shell entry
	b 1b

