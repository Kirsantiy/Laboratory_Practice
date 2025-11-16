#include "init.h"

// --- Функция задержки, HSI = 16 МГц
void Delay(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 16000; i++)
    {
        __asm__("nop");
    }
}

void GPIO_Init(void)
{
    // 1. Включение тактирования портов GPIOA, GPIOB (RCC_AHB1ENR)
    *(uint32_t *)(RCC_AHB1ENR_ADDR) |= RCC_GPIOA_EN;
    *(uint32_t *)(RCC_AHB1ENR_ADDR) |= RCC_GPIOB_EN;

    // 2.1 Настройка GPIOA в режим входа (0-2 пины, input - состояние по умолчанию)
    *(uint32_t *)(GPIOA_MODER_ADDR) |= GPIOA_MODER_INPUT;

    // 2.2 Настройка GPIOB в режим выхода (3-8 пины)
    *(uint32_t *)(GPIOB_MODER_ADDR) = 0x00000000UL;
    *(uint32_t *)(GPIOB_MODER_ADDR) |= GPIOB_MODER_OUTPUT;

    // 3. Настройка на Push-Pull (всё переводим в положение выкл)
    *(uint32_t *)(GPIOA_OTYPER_ADDR) = 0x00000000UL;
    *(uint32_t *)(GPIOB_OTYPER_ADDR) = 0x00000000UL;

    // 4. Настройка PU/PD резисторов (GPIOA - PD, GPIOB - выкл)
    *(uint32_t *)(GPIOA_PUPDR_ADDR) |= GPIOA_PUPD;
    *(uint32_t *)(GPIOB_PUPDR_ADDR) |= GPIOB_PUPD;

    // 5. Настройка BSRR (GPIOB) - изначально отключаем светодиоды
    *(uint32_t *)(GPIOB_BSRR_ADDR) = (LED1_RESET | LED2_RESET | LED3_RESET | 
                                     LED4_RESET | LED5_RESET | LED6_RESET);
}
