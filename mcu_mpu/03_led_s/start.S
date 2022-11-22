
.text
.global _start
_start:
    
    /* 1. enable gpio5 clk - default enable */
    /* 2. set pin iomux to gpio */
    LDR r0, =(0x2290000+0x14)
    LDR r1, [r0]
    ORR r1, r1, #5
    STR r1, [r0]
    
    /* set gpio direction */
    LDR r0, =(0x20ac004)
    LDR r1, [r0]
    ORR r1, r1, #(1<<3)
    STR r1, [r0]
    
    /* gpio data */
    LDR r2, =(0x20ac000)
    
Loop:
    /* led off */
    LDR r1, [r2]
    ORR r1, r1, #(1<<3)
    STR r1, [r2]
    
    /* delay */
    LDR r0, =1000000
    BL Delay
    
    /* led on(low enable) */
    LDR r1, [r2]
    BIC r1, r1, #(1<<3)
    STR r1, [r2]
    
    /* delay */
    LDR r0, =1000000
    BL Delay
    
    //BL Loop

Delay:
    SUBS r0, r0, #1
    BNE Delay
    MOV pc, lr