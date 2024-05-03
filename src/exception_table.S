#define __ASSEMBLY__
#include "exception_table.h"

// save general registers to stack
.macro save_all
	sub sp, sp, #272
	stp x0, x1, [sp ,16 * 0]
	stp x2, x3, [sp ,16 * 1]
	stp x4, x5, [sp ,16 * 2]
	stp x6, x7, [sp ,16 * 3]
	stp x8, x9, [sp ,16 * 4]
	stp x10, x11, [sp ,16 * 5]
	stp x12, x13, [sp ,16 * 6]
	stp x14, x15, [sp ,16 * 7]
	stp x16, x17, [sp ,16 * 8]
	stp x18, x19, [sp ,16 * 9]
	stp x20, x21, [sp ,16 * 10]
	stp x22, x23, [sp ,16 * 11]
	stp x24, x25, [sp ,16 * 12]
	stp x26, x27, [sp ,16 * 13]
	stp x28, x29, [sp ,16 * 14]
	mrs x21, sp_el0
	stp x30, x21, [sp, 16 * 15]
	mrs x21, elr_el1
    mrs x22, spsr_el1
    stp x21, x22, [sp, #16 * 16]
.endm
// load general registers from stack
.macro load_all
	ldp x0, x1, [sp ,16 * 0]
	ldp x2, x3, [sp ,16 * 1]
	ldp x4, x5, [sp ,16 * 2]
	ldp x6, x7, [sp ,16 * 3]
	ldp x8, x9, [sp ,16 * 4]
	ldp x10, x11, [sp ,16 * 5]
	ldp x12, x13, [sp ,16 * 6]
	ldp x14, x15, [sp ,16 * 7]
	ldp x16, x17, [sp ,16 * 8]
	ldp x18, x19, [sp ,16 * 9]
	ldp x20, x21, [sp ,16 * 10]
	ldp x22, x23, [sp ,16 * 11]
	ldp x24, x25, [sp ,16 * 12]
	ldp x26, x27, [sp ,16 * 13]
	ldp x28, x29, [sp ,16 * 14]
	ldp x30, x21, [sp, 16 * 15]
	msr sp_el0, x21
	ldp x21, x22, [sp, 16*16]
	msr elr_el1, x21
	msr spsr_el1, x22
	add sp, sp, #272
.endm

.align 11
.global exception_vector_table

exception_vector_table:
	/* Current EL with SP0
       	當前系統運行在EL1時使用EL0的棧指針SP
       	這是一種異常錯誤的類型
    */
	b el1_sync_invalid
  	.align 7
	b el1_irq_invalid
	.align 7
	b el1_fiq_invalid
	.align 7
	b el1_error_invalid
	.align 7
	/* Current EL with SPx
		當前系統運行在EL1時使用EL1的棧指針SP
		這說明系統在內核態發生了異常
 	*/
	b el1_sync_invalid
	.align 7
	b el1_irq_invalid
	.align 7
	b el1_fiq_invalid
	.align 7
	b el1_error_invalid
	.align 7
	 /* Lower EL using AArch64
		在用戶態的aarch64的程序發生了異常
	*/
	b el0_sync_invalid
	.align 7
	b el0_irq_invalid
	.align 7
	b el0_fiq_invalid
	.align 7
	b el0_error_invalid
	.align 7
	/* Lower EL using AArch32
		在用戶態的aarch32的程序發生了異常
	*/
	b el0_sync_invalid
	.align 7
	b el0_irq_invalid
	.align 7
	b el0_fiq_invalid
	.align 7
	b el0_error_invalid
	.align 7
el1_sync_invalid: //exception handler
	save_all
	mrs x0, ELR_EL1
	mrs x1, ESR_EL1
	bl show_exception_message
	load_all
	eret
el1_irq_invalid:
	save_all
	bl irq_sp_switch
	bl irq_return
	load_all
	eret
el1_fiq_invalid:
	save_all
	mrs x0, ELR_EL1
	mrs x1, ESR_EL1
	bl show_exception_message
	load_all
	eret
el1_error_invalid:
	save_all
	mrs x0, ELR_EL1
	mrs x1, ESR_EL1
	bl show_exception_message
	load_all
	eret
el0_sync_invalid: //(el0 -> el1) (AARH-64)
	save_all
	mrs x0, ELR_EL1
	mrs x1, ESR_EL1
	mov x2, sp
	bl syscall_table
	load_all
	eret
el0_irq_invalid:
	save_all
	bl irq_sp_switch
	bl irq_return
	load_all
	eret
el0_fiq_invalid:
	save_all
	mrs x0, ELR_EL1
	mrs x1, ESR_EL1
	bl show_exception_message
	load_all
	eret
el0_error_invalid:
	save_all
	mrs x0, ELR_EL1
	mrs x1, ESR_EL1
	bl show_exception_message
	load_all
	eret

.global ret_child_process
ret_child_process:
	load_all
	eret