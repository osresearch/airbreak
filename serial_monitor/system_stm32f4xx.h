#ifndef __SYSTEM_STM32F4XX_H
#define __SYSTEM_STM32F4XX_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

/* Clock PLLs for 407 chip */
#if defined (STM32F407xx) || defined (STM32F429xx)
// Main PLL = N * (source_clock / M) / P
// HSE = 8 Mhz
// fCLK =   N * (8Mhz / M) / P
#define PLL_M   8
#define PLL_Q   7
#define PLL_P   2
#endif

/* stm32f407 runs at 168Mhz max */
#if defined (STM32F407xx)
#define PLL_N   336
#endif

/* stm32f429 runs at 180Mhz max */
#if defined (STM32F429xx)
#define PLL_N   360
#endif

void __init_data(void);
void __Reset_Handler(void);
void SystemInit(void);
void set_sysclk_to_168(void);
/* bring main */
extern int main(void);

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_STM32F4XX_H */
