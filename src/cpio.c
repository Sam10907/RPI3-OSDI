#include "cpio.h"

char *initramfs_addr = (char*)0xffff000008000000;

int my_atoi(const char *nptr, int size){
    int i, res = 0;
    for ( i = 0; i < size ; i++)
    {
        res = res *16 + (nptr[i] - '0');
    }
    return res;
}

void cat(const char *content, int size){
    int i = 0;
    for ( ; i < size; i++)
    {
       uart_write(content[i]);
    }
}

void initramfs_main(){
    /* ls and cat*/
    char *next_file = initramfs_addr;
    struct cpio_header *file_header =  (struct cpio_header*)(next_file + sizeof(struct cpio_header));
    char *file_name = (char*)file_header + sizeof(struct cpio_header) - 2;
    while (strcmp(file_name, "000TRAILER!!!")){
        uart_printf("%s\n", file_name);
        char *file_content = file_name + my_atoi(file_header -> c_namesize,sizeof(file_header -> c_namesize));
        int file_size = my_atoi(file_header -> c_filesize, sizeof(file_header -> c_filesize));
        cat(file_content, file_size);
        file_header =  (struct cpio_header*)(file_content + file_size);
        file_name = (char*)file_header + sizeof(struct cpio_header) - 2;
    }
}
