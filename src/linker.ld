SECTIONS
{
  . = 0xFFFF000000000000;
  . += 0x80000;
  __kernel_start = .;
  .text :
  {
    KEEP(*(.text.boot))
    *(.text)
  }
  .data :
  {
    *(.data)
  }

  . = ALIGN(0x8);
	.bss : 
  { 
    bss_begin = .;
    *(.bss*)
    bss_end = .;
  } 
  . = ALIGN(0x1000);
  pg_dir = .;
  .data.pgd :
  {
    . += 515 * (1 << 12);
  }
  . = ALIGN(0x1000);
  __kernel_end = .;
}
