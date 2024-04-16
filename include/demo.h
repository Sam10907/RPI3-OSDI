#define SPSR_EL1_MASK (0b0000 << 6)
#define SPSR_EL1_EL0 (0b0000 << 0)
#define SPSR_EL1_VALUE (SPSR_EL1_MASK | SPSR_EL1_EL0)
#define SP_ADDR (0x40000)

void example_code();
void demo_syscall();