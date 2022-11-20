
void delay(volatile int d)
{
    while(d--);
}

int main(void)
{
    volatile unsigned int *pReg;
    /* enable gpio5 clk - default enable */
    
    /* set pin iomux to gpio */
    pReg = (volatile unsigned int *)(0x2290000 + 0x14);
    *pReg = *pReg | 5;
    
    /* set gpio direction */
    pReg = (volatile unsigned int *)(0x20ac004);
    *pReg = *pReg | (1<<3);
    
    /* gpio data */
    pReg = (volatile unsigned int *)(0x20ac000);
    
    while(1){
        
        *pReg = *pReg | (1<<3);
        delay(1000000);
        
        *pReg &= ~(1<<3);
        delay(1000000);
    }
}