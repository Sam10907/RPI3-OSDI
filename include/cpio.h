#ifndef __CPIO_H_
#define __CPIO_H_

#include "uart.h"
#include "my_string.h"

struct cpio_header
{
    char    c_magic[6];
    char    c_ino[8];
    char    c_mode[8];
    char    c_uid[8];
    char    c_gid[8];
    char    c_nlink[8];
    char    c_mtime[8];
    char    c_filesize[8];
    char    c_devmajor[8];
    char    c_devminor[8];
    char    c_rdevmajor[8];
    char    c_rdevminor[8];
    char    c_namesize[8];
    char    c_check[8];
    char    padding[2]; //align 8
};

int my_atoi(const char *, int);
void cat(const char *, int);
void initramfs_main();

#endif
