
#include "uart.h"

// ??? IOMUXC_SW_PAD_CTL_PAD_UART1_TX_DATA
// ??? UFCR
// ??? UCR2 bit[14]

//#define IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA 0x020E0084
//#define IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA 0x020E0088
//#define IOMUXC_UART1_RX_DATA_SELECT_INPUT   0x020E0624
//#define CCM_CSCDR1  0x020C4024
//#define CCM_CCGR5   0x020C407C


typedef struct {
    volatile unsigned int URXD;   //0x02020000
    unsigned char Reserve1[60];//4*16-4=60
    volatile unsigned int UTXD;   //0x02020040
    unsigned char Reserve2[60];//4*16-4=60
    volatile unsigned int UCR1;   //0x02020080
    volatile unsigned int UCR2;   //0x02020084
    volatile unsigned int UCR3;   //0x02020088
    volatile unsigned int UCR4;   //0x0202008C
    volatile unsigned int UFCR;   //0x02020090
    volatile unsigned int USR1;   //0x02020094
    volatile unsigned int USR2;   //0x02020098
    volatile unsigned int UESC;
    volatile unsigned int UTIM;
    volatile unsigned int UBIR;
    volatile unsigned int UBMR;
    volatile unsigned int UBRC;
    volatile unsigned int ONEMS;
    volatile unsigned int UTS;
    volatile unsigned int UMCR;
    
}uart_t;
       

// 確定uart pin
// set uart clock
// enable uart clock
// set uart param: baurd_rate, ...


void uart_init(void)
{
    volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA;
    volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA;
    volatile unsigned int *IOMUXC_UART1_RX_DATA_SELECT_INPUT;
    volatile unsigned int *CCM_CSCDR1;
    volatile unsigned int *CCM_CCGR5;
    uart_t *pUart1 = (uart_t *)(0x02020000);
    
    IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA = (volatile unsigned int*)(0x020E0084);
    IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA = (volatile unsigned int*)(0x020E0088);
    IOMUXC_UART1_RX_DATA_SELECT_INPUT = (volatile unsigned int*)(0x020E0624);
    CCM_CSCDR1 = (volatile unsigned int*)(0x020C4024);
    CCM_CCGR5 = (volatile unsigned int*)(0x020C407C);
    
    
    //uart freq=80MHz, divider=1
    *CCM_CSCDR1 &= ~((1<<6) | (0x3f));
    
    // enable uart clock
    // CCM_CCGR5 bit[25:24] = 0b11
    *CCM_CCGR5  |= (3<<24);
    
    // 設置用作uart
    // RxData[3:0]=0 TxData[3:0]=0
    *IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA &= ~(0xf);
    *IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA &= ~(0xf);
    
    //
    *IOMUXC_UART1_RX_DATA_SELECT_INPUT |= 3;
    
    
    // uart參數: baurd_rate, data bit, stop bit, check bit
    /* BaudRate = RefFreq/(16 * (UBMR+1)/(UBIR+1))
     * UBIR = 15
     * 115200 = 80MHz / (16*(UBMR+1)/(16) )
     * 80,000,000/115200 = (UBMR+1)
     * (UBMR+1) = 694.444
     * UBMR = 693.444
     */
    pUart1->UFCR |= (5<<7);
    pUart1->UBIR = 15;
    pUart1->UBMR = 693;
    
    
    /* bit[12]: CTS, bit[8]: 0-disable Parity
     * bit[6]: 0-STOP 1bit, bit[5]: 1-8bit data, 
     * bit[2]: 1-txEn, bit[1]: 1-rxEn, bit[0]: 0-SRST => Not to do sw reset
     */
    pUart1->UCR2 &= ~((1<<12)|(1<<8)|(1<<6));
    pUart1->UCR2 |= (1<<5)|(1<<2)|(1<<1);
    
    pUart1->UCR3 |= (1<<2);
    
    // 使能Uart
    pUart1->UCR1 |= (1<<0);
    
}

int getchar(void)
{
    uart_t *pUart1 = (uart_t *)(0x02020000);
    
    //狀態寄存器 wait til got rx data ready
    while((pUart1->USR2 & (1<<0)) == 0);
    
    //數據寄存器
    return pUart1->URXD;
}

void putchar(char c)
{
    uart_t *pUart1 = (uart_t *)(0x02020000);
    
    //狀態寄存器 wait til tx complete
    while((pUart1->USR2&(1<<3)) == 0);
    
    //數據寄存器
    pUart1->UTXD = c;
}
