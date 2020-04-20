#include "monitor_addr.h"
#include "stm32f4xx.h"

//Use same calling as function we override
int start(int nothing)
{
    //Turn OFF DMA (todo - could we use this instead)
    USART3->CR3 = 0; //Everything else is 0 in this reg anyway

    //Turn on USART3 interrupt
    NVIC_SetPriority(USART3_IRQn, 0x03);
    NVIC_EnableIRQ(USART3_IRQn);
    
    // enable USART3 RXNEIE, bit 5
    USART3->CR1 |= USART_CR1_RXNEIE;
    
    nothing=nothing;
    return 0;
}

/* The following was used for independant testing - on AirSense setup is already done. Left in for completeness*/
#if 0
void start(void)
{
    // enable USART3 clock, bit 18 on APB1ENR
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    // enable GPIOB clock, bit 0 on AHB1ENR
    RCC->AHB1ENR |= (1 << 1);

    // set pin modes as alternate mode 7 (pins 10 and 11)
    GPIOB->MODER &= 0xFF0FFFFF; // Reset bits to clear old values
    GPIOB->MODER |= 0x00A00000; // Set pin 10/11 to alternate func. mode (0b10)

    // set pin modes as high speed
    GPIOB->OSPEEDR |= 0x00A00000; // Set pin 10/11 to high speed mode (0b10)

    // choose AF7 for USART3 in Alternate Function registers
    GPIOB->AFR[1] |= (0x7 << 8); // for pin 10
    GPIOB->AFR[1] |= (0x7 << 12); // for pin 11

    // usart3 tx enable, TE bit 3
    USART3->CR1 |= (1 << 3);

    // usart3 rx enable, RE bit 2
    USART3->CR1 |= (1 << 2);


    /** THE FOLLOWING IS NOT ACCURATE **/
    // baud rate = fCK / (8 * (2 - OVER8) * USARTDIV)
    //   for fCK = 42 Mhz, baud = 115200, OVER8 = 0
    //   USARTDIV = 42Mhz / 115200 / 16
    //   = 22.7864 22.8125
    // we can also look at the table in RM0090
    //   for 42 Mhz PCLK, OVER8 = 0 and 115.2 KBps baud
    //   we need to program 22.8125
    // Fraction : 16*0.8125 = 13 (multiply fraction with 16)
    // Mantissa : 22
    // 12-bit mantissa and 4-bit fraction
    USART3->BRR |= (22 << 4);
    USART3->BRR |= 13;

    // enable usart3 - UE, bit 13
    USART3->CR1 |= (1 << 13);
    
    *((uint32_t *)ADDR_BADCNT) = 0;
    
    NVIC_SetPriority(USART3_IRQn, 0x03);
    NVIC_EnableIRQ(USART3_IRQn);
    
    // enable USART3 RXNEIE, bit 5
    USART3_RXNEIE_enable();
    
    return
}
#endif