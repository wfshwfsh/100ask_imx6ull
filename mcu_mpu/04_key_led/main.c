
void delay(volatile int d)
{
    while(d--);
}

int main(void)
{
    volatile unsigned int *pRegLed, *pRegKey;
    /* enable gpio5 clk - default enable */
    
    /* set pin iomux to gpio */
    pRegLed = (volatile unsigned int *)(0x2290000 + 0x14);
    *pRegLed = *pRegLed | 5;
    
    pRegKey = (volatile unsigned int *)(0x2290000 + 0x0C);
    *pRegKey = *pRegKey | 5;
    
    /* set gpio direction */
    pRegLed = (volatile unsigned int *)(0x20ac004);
    *pRegLed = *pRegLed | (1<<3);
    
    pRegKey = (volatile unsigned int *)(0x20ac004);
    *pRegKey = *pRegKey & ~(1<<1);
    
    /* gpio data */
    pRegLed = (volatile unsigned int *)(0x20ac000);
    pRegKey = (volatile unsigned int *)(0x20ac000);
    
    while(1){
        
        if(*pRegKey & (1<<1)){
            /* no press, led on */
            *pRegLed &= ~(1<<3);
        }else{
            /* press, led off */
            *pRegLed = *pRegLed | (1<<3);
        }
        
        delay(1000);
    }
}