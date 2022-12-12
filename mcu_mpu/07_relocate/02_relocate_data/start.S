
.text
.global _start
_start:

    ldr sp, =(0x80000000+0x100000)
	
	/* relocate */
	/* src, dst, len */
	
	/* dst:  */
	ldr r0, =__rodata_start
	
	/* src: __rodata_start - (B(鏈接地址_start) - A(加載地址_start)) */
	adr r3, _start  //A: load addr
	ldr r2, =_start //B: link addr
	
	sub r2, r2, r3
	sub r1, r0, r2
	
	/* len:  */
	ldr r2, =__bss_start
	sub r2, r2, r0
	
	/* memcpy */
	bl memcpy
	
    bl main
