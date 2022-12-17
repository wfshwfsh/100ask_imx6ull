#include "uart.h"
#include "key.h"
#include "gic.h"

#define IRQ_NO__GPIO4_0_15 (72+32)

/** GPIO - Register Layout Typedef */
typedef struct {
  volatile unsigned int DR;                                /**< GPIO data register, offset: 0x0 */
  volatile unsigned int GDIR;                              /**< GPIO direction register, offset: 0x4 */
  volatile unsigned int PSR;                               /**< GPIO pad status register, offset: 0x8 */
  volatile unsigned int ICR1;                              /**< GPIO interrupt configuration register1, offset: 0xC */
  volatile unsigned int ICR2;                              /**< GPIO interrupt configuration register2, offset: 0x10 */
  volatile unsigned int IMR;                               /**< GPIO interrupt mask register, offset: 0x14 */
  volatile unsigned int ISR;                               /**< GPIO interrupt status register, offset: 0x18 */
  volatile unsigned int EDGE_SEL;                          /**< GPIO edge select register, offset: 0x1C */
} GPIO_Type;


//key2: GPIO4_IO14
// ...
// GPIO4_ICR1: 20A_800C
// ...
// GPIO4 baseAddr: 20A_8000

//led2: GPIO5_IO03

void key_init(void)
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
    
	GPIO_Type *gpio4 = (GPIO_Type *)0x020A8000;
	
	gpio4->GDIR &= ~(1<<14);
	
	/* clear ISR by writing 1 */
	gpio4->ISR |= 1<<14;
	clear_gic_irq(IRQ_NO__GPIO4_0_15);
	
	/* set interrupt type: rising/falling/level */
	/* set EDGE_SEL for both rising & falling, and replcae ICR1 */
	gpio4->EDGE_SEL |= (1<<14);
	
	/* set interrupt mask */
	gpio4->IMR |= (1<<14); //1: unmask, 0: mask
	
	/* enable irq */
	gic_enable_irq(IRQ_NO__GPIO4_0_15);
}

void do_irq_c(void)
{
	puts("do_irq_c\r\n");
	
    volatile unsigned int *pRegLed;
    /* gpio data */
    pRegLed = (volatile unsigned int *)(0x20AC000);
	GPIO_Type *gpio4 = (GPIO_Type *)0x020A8000;
	
	/* 分辨中斷 */
	int irq_no = get_gic_irq();
	
	/* 調用中斷處理 */
	if(IRQ_NO__GPIO4_0_15 == irq_no){
		
		if(gpio4->DR & (1<<14)){
			/* no press, led on */
			puts("KEY2 released!\n\r");
			*pRegLed &= ~(1<<3);
		}else{
			/* press, led off */
			puts("KEY2 pressed!\n\r");
			*pRegLed = *pRegLed | (1<<3);
		}
		gpio4->ISR |= 1<<14;
	}
	
	/* 清中斷 */
	clear_gic_irq(irq_no);
}
