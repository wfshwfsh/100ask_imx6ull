//GPIO4_CLK_ENABLE 0x20E01B0 => bit[13:12] 0b11
//GPIO4_DR    0x20A8000
//GPIO4_GDIR  0x20A8004
//GPIO4_PSR   0x20A8008
//GPIO4_IOMUX 0x20E0000+0x01B0


void delay(volatile int d)
{
    while(d--);
}

int main(void)
{
    volatile unsigned int *pRegLed, *pRegKey;
    /* enable gpio5 clk - default enable */
    /* enable gpio4 clk b[13:12]=0b11 */
    pRegKey = (volatile unsigned int *)(0x20C4000 + 0x74);
    *pRegKey |= (3<<12);
    
    /* set pin iomux to gpio */
    pRegLed = (volatile unsigned int *)(0x2290000 + 0x14);
    *pRegLed |= 5;
    
    pRegKey = (volatile unsigned int *)(0x20E0000 + 0x01B0);
    *pRegKey &= ~(0xf);
    *pRegKey |= 5;
    
    /* set gpio direction */
    pRegLed = (volatile unsigned int *)(0x20AC004);
    *pRegLed |= (1<<3);
    
    pRegKey = (volatile unsigned int *)(0x20A8004);
    *pRegKey &= ~(1<<14);
    
    /* gpio data */
    pRegLed = (volatile unsigned int *)(0x20AC000);
    pRegKey = (volatile unsigned int *)(0x20A8000);
    
    while(1){
        
        if(*pRegKey & (1<<14)){
            /* no press, led on */
            *pRegLed &= ~(1<<3);
        }else{
            /* press, led off */
            *pRegLed = *pRegLed | (1<<3);
        }
        
        delay(1000);
    }
}