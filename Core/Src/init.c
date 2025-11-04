#include "init.h"

void GPIO_Init(void)
{
    // 1. Включение тактирования порта GPIOC (RCC_AHB1ENR, Bit 2)
    *(uint32_t*)(RCC_AHB1ENR_ADDR) |= RCC_GPIOC_EN; 

    // 2. Настройка PC13 в режим вывода (Output mode: MODER13 = 01b)
    *(uint32_t*)(GPIOC_BASE + GPIOC_MODER_OFFSET) &= ~GPIOC_MODER_CLEAR; 
    *(uint32_t*)(GPIOC_BASE + GPIOC_MODER_OFFSET) |= GPIOC_MODER_OUTPUT; 

    // 3. Настройка на Push-Pull (OTYPER13 = 0b)
    *(uint32_t*)(GPIOC_BASE + GPIOC_OTYPER_OFFSET) &= ~(1 << 13);

    // 4. Отключение PU/PD резисторов (PUPDR13 = 00b)
    *(uint32_t*)(GPIOC_BASE + GPIOC_PUPDR_OFFSET) &= ~GPIOC_MODER_CLEAR; 
}

