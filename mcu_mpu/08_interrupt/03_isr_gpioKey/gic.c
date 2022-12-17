#include "gic.h"

#define __GIC_PRIO_BITS 5

static GIC_Type* get_gic_base(void)
{
    GIC_Type *dst;
	__asm volatile ("mrc p15, 4, %0, c15, c0, 0" : "=r" (dst));
	return dst;
}

void gic_init(void)
{
    uint32_t i, irq_num;
    GIC_Type *gic = get_gic_base();
    
    /* get gic support max interrupts */
    irq_num = (gic->D_TYPER & 0x1FUL) + 1;

    /* On POR, all SPI is in group 0, level-sensitive and using 1-N model */
    /* Disable all PPI, SGI and SPI */
    for (i = 0; i < irq_num; i++)
        gic->D_ICENABLER[i] = 0xFFFFFFFFUL;
    
    /* 这些中断, 都发给group0 */
    for (i = 0; i < irq_num; i++)
        gic->D_IGROUPR[i] = 0x0UL;
    
    /* SPI都發給CPU0 */
    /* set SPI intr target cpu - cpu core 0 */
    for (i = 32; i < irq_num; i++)
        gic->D_ITARGETSR[i] = 0x01UL;
    
    /* all spi is level sensitive: 0-level, 1-edge */
	/* it seems level and edge all can work */
	/* 设置GIC内部的中断触发类型 */
	for (i = 2; i < irq_num << 1; i++)
	  gic->D_ICFGR[i] = 0x01010101UL;
    
    /* Make all interrupts have higher priority */
    gic->C_PMR = (0xFFUL << (8 - __GIC_PRIO_BITS)) & 0xFFUL;

    /* No subpriority, all priority level allows preemption */
    gic->C_BPR = 7 - __GIC_PRIO_BITS;

    /* Enable group0 distribution */
    /* 使能:   Distributor可以给CPU interfac分发中断 */
    gic->D_CTLR = 1UL;

    /* Enable group0 signaling */
    /* 使能:	 CPU interface可以给processor分发中断 */
    gic->C_CTLR = 1UL;
}

void gic_enable_irq(int nr)
{
    GIC_Type *gic = get_gic_base();
    gic->D_ISENABLER[nr >> 5] = (uint32_t)(1UL << (nr & 0x1FUL));
}

void gic_disable_irq(int nr)
{
    GIC_Type *gic = get_gic_base();
    gic->D_ICENABLER[nr >> 5] = (uint32_t)(1UL << (nr & 0x1FUL));
}

int get_gic_irq(void)
{
    GIC_Type *gic = get_gic_base();
    
    //core_ca7.h use: gic->C_IAR & 0x1FFFUL => 3bit for CPUID
    return (gic->C_IAR & 0x3FFUL);
}

void clear_gic_irq(int nr)
{
    GIC_Type *gic = get_gic_base();
    gic->C_EOIR |= nr & 0x3FFUL;
	//gic->C_EOIR = nr;
}
