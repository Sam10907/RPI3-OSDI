SECTIONS
{
	. = 0x0;
	.text.entry : {*(.text.entry)}
	.text : {*(.text)}
	.rodata : {*(.rodata)}
	.data : {*(.data)}
	. = ALIGN(16);
	.bss : {*(.bss*)}
}
