
.text
.global _start
_start:

    /* 設置棧 */
	ldr sp, =(0x80000000+0x100000)
	
    /* src: */
    adr r0, _start  // load addr
    bl SystemInit
    
	
	/* 調用main */
    #bl main     //相對跳轉指令
	ldr pc, =main //絕對跳轉指令