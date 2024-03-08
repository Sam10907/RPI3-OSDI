#ifndef __REG_H_
#define __REG_H_

//GPIO register
#define BASE ((volatile unsigned int) 0x3f000000)
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

#endif
