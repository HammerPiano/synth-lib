/*
File: linkerscript.ld

Purpose: linker script for linking complied files

Author: HammerPiano

Thanks to the talented people that created the Ac6 System workbench IDE, for creating a ld file
we alot of explanations
*/

/* entry point of the program, the reset handler */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20005000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */
/* memory regions */
MEMORY
{
	RAM (rxw)		: org = 0x20000000, len = 20K
	/*FLASH (rx)		: org = 0x08000000, len = 60K
	BOOT  (rx)		: org = 0x0800F000, len = 4K*/
	FLASH (rx)		: org = 0x80000000, len = 64K

}

SECTIONS
{
	/* isr_vector which contains the startup code, and basic interrupt handling */
	.isr_vector :
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} >FLASH

	.text :
	{
		
		EXCLUDE_FILE (*boot.c) *(.text)		/* .text sections (code) */
		EXCLUDE_FILE (*boot.c) *(.text*)	/* .text* sections (code) */
		*(.glue_7)		/* glue arm to thumb code */
		*(.glue_7t)		/* glue thumb to arm code */
		*(.eh_frame)	/* exception handling frame http://web.archive.org/web/20130111101034/http://blog.mozilla.org/respindola/2011/05/12/cfi-directives */
		. = ALIGN(4);
		_etext = .;
	} >FLASH
	
	_sidata = LOADADDR(.data);

	.data :
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data)
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} >RAM AT> FLASH

	. = ALIGN(4);
	.bss : 
	{
		_sbss = .;
		__bss_start__ = _sbss;
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = _ebss;
	}
	/* User_heap_stack section, used to check that there is enough RAM left */
	._user_heap_stack :
	{
		. = ALIGN(8);
		PROVIDE ( end = . );
		PROVIDE ( _end = . );
		. = . + _Min_Heap_Size;
		. = . + _Min_Stack_Size;
		. = ALIGN(8);
	} >RAM



	/* Remove information from the standard libraries */
	/DISCARD/ :
	{
		libc.a ( * )
		libm.a ( * )
		libgcc.a ( * )
	}
	/*
	.boot : 
	{
		_sboot = .
		boot.c(.text)
		. = ALIGN(4);
		_eboot = .
	} >BOOT*/
	
}