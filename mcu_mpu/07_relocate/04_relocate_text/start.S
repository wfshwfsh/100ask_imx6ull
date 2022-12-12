
.text
.global _start
_start:

    /* 設置棧 */
	ldr sp, =(0x80000000+0x100000)
	
	/* relocate text/rodata/rwdata */
	/* dst: */
	ldr r0, =_start // link addr
	
	/* src: */
	adr r1, _start  // load addr
	
	/* len:  */
	ldr r3, =__bss_start
	sub r2, r3, r0
	
	/* memcpy */
	bl memcpy
	
	
	/* clear bss */
	ldr r0, =__bss_start
	mov r1, #0
	ldr r2, =__bss_end
	sub r2, r2, r0
	bl memset  /* r0:dest, r1:val, r2:len */
	
	/* 調用main */
    #bl main     //相對跳轉指令
	ldr pc, =main //絕對跳轉指令