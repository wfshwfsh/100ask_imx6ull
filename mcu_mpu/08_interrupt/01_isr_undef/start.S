#define STACK_BASE (0x80000000+0x100000)
#define STACK_SIZE 

.text
.global _start
_start:
    b reset
    ldr pc, =isr_undef
	.word 0  // ldr	pc, _software_interrupt
	.word 0  // ldr	pc, _prefetch_abort
	.word 0  // ldr	pc, _data_abort
	.word 0  // ldr	pc, _not_used
	.word 0  // ldr	pc, _irq
	.word 0  // ldr	pc, _fiq
    
reset:
    
    /* 設置棧 */
	ldr sp, = STACK_BASE
	
    /* src: */
    adr r0, _start  // load addr
    bl SystemInit
    
    bl uart_init
    
    /* 设置异常向量表基地址 : VBAR */
    ldr r0, =_start
    MCR p15, 0, r0, c12, c0, 0
	
    /* 觸發中斷 */
    .word  0xffffffff
    
	/* 調用main */
    #bl main     //相對跳轉指令
	ldr pc, =main //絕對跳轉指令
    
isr_undef:
    
    /* 保存現場 */
    ldr sp, =STACK_BASE-STACK_SIZE
    stmdb sp!, {r0-r3,r12,lr}
    
    /* 調用中斷處理函數 */
    bl do_undef_irq
    
    /* 恢復現場 */
    ldmia sp!, {r0-r3,r12,pc}^
