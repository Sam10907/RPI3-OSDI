#include "schedule.h"
#include "mm.h"
#include "uart.h"
#include "cpio.h"

void boot_main(){
	uart_printf("\n\n _  _  ___ _____ _   _  ___  ___ ___ ___ \n");
    uart_printf("| \\| |/ __|_   _| | | |/ _ \\/ __|   \\_ _|\n");
    uart_printf("| .` | (__  | | | |_| | (_) \\__ \\ |) | | \n");
    uart_printf("|_|\\_|\\___| |_|  \\___/ \\___/|___/___/___|\n\n");
    uart_printf("Init RAM disk...\n");
    initramfs_main();
	mm_init();
    task_init();
    schedule_init();
}
