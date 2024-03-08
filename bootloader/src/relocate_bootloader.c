extern unsigned char _begin, _end, __boot_loader;

__attribute__((section(".text.relocate"))) void relocate() {
    unsigned long kernel_size = (&_end - &_begin);
    unsigned char *new_bl = (unsigned char *)&__boot_loader;
    unsigned char *bl = (unsigned char *)&_begin;

    while (kernel_size--) {
        *new_bl++ = *bl;
        bl++;
    }
    void (*start)(void) = (void *)&__boot_loader;
    //Clear the memory location where the original bootloader program is located to 0, because the os kernel will be moved here by the bootloader.
    kernel_size = (&_end - &_begin);
    bl = (unsigned char *)&_begin;
    while (kernel_size--) {
        *bl++ = 0;
    }
    start();
}
