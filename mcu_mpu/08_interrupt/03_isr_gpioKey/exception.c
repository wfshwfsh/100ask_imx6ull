#include "uart.h"
#include "string.h"
#include "exception.h"

void do_undef_irq(void)
{
    puts("do_undef_irq\r\n");
}

void do_svc_irq(void)
{
    puts("do_svc_irq\r\n");
}