#include "uart.h"

int main(void)
{
    char c;
    uart_init();
    
    putchar('H');
    putchar('E');
    putchar('L');
    putchar('L');
    putchar('O');
    putchar('\r');
    putchar('\n');
    
    while(1){
        c = getchar();
        putchar(' ');
        putchar(c);
    }
    
    return 0;
}