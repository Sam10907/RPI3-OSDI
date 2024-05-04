#ifndef __REG_H_
#define __REG_H_

#define BASE (0xffff000000000000 | 0x3f000000)

//GPIO register
#define GPFSEL1 ((volatile unsigned int*)(BASE + 0x200004))
#define GPPUD ((volatile unsigned int*)(BASE + 0x200094))
#define GPPUDCLK0 ((volatile unsigned int*)(BASE + 0x200098))
#define GPPUDCLK1 ((volatile unsigned int*)(BASE + 0x20009c))

//miniUART register
#define AUXENB ((volatile unsigned int*) (BASE + 0x215004))
#define AUX_MU_CNTL_REG ((volatile unsigned int*) (BASE + 0x215060))
#define AUX_MU_IER_REG ((volatile unsigned int*) (BASE + 0x215044))
#define AUX_MU_LCR_REG ((volatile unsigned int*) (BASE + 0x21504c))
#define AUX_MU_MCR_REG ((volatile unsigned int*) (BASE + 0x215050))
#define AUX_MU_BAUD ((volatile unsigned int*) (BASE + 0x215068))
#define AUX_MU_IIR_REG ((volatile unsigned int*) (BASE + 0x215048))
#define AUX_MU_LSR_REG ((volatile unsigned int*) (BASE + 0x215054))
#define AUX_MU_IO_REG ((volatile unsigned int*) (BASE + 0x215040))

//arm local peripherals
#define ARM_PERIPHERALS_BASE (0xffff000000000000 | 0x40000000)
#define CORE0_TIMER_IRQ_CTRL (ARM_PERIPHERALS_BASE + 0x40)
#define CORE0_TIMER_IRQ_SRC ((volatile unsigned int*)(ARM_PERIPHERALS_BASE + 0x60))

#endif
