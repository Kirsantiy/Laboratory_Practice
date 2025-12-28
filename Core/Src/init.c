#include "init.h"

// Глобальный счетчик миллисекунд
volatile uint32_t g_millis = 0;
static volatile uint32_t systick_counter = 0;

// --- Функция задержки, HSI = 16 МГц
void Delay(uint32_t ms)
{
    // uint32_t start = Millis();
    // while ((Millis() - start) < ms)
    // {
    //     // Обновляем счетчик времени во время задержки
    //     UpdateMillis();
    // }

    for (uint32_t i = 0; i < ms * 16000; i++)
    {
        __asm__("nop");
    }
}

// --- Функция получения текущего времени в миллисекундах ---
uint32_t Millis(void)
{
    return g_millis;
}

void UpdateMillis(void)
{
    static uint32_t last_time_check = 0;
    static uint32_t cycle_count = 0;
    
    cycle_count++;
    
    if (cycle_count >= 55)
    {
        g_millis++;
        cycle_count = 0;
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
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x40; // задаем режим выхода на светодиод PB3

    // 3. Настройка на Push-Pull (всё переводим в положение выкл)
    *(uint32_t *)(GPIOA_OTYPER_ADDR) = 0x00000000UL;
    *(uint32_t *)(GPIOB_OTYPER_ADDR) = 0x00000000UL;

    // 4. Настройка PU/PD резисторов (GPIOA - PD, GPIOB - выкл)
    *(uint32_t *)(GPIOA_PUPDR_ADDR) |= GPIOA_PUPD;
    *(uint32_t *)(GPIOB_PUPDR_ADDR) |= GPIOB_PUPD;

    // 5. Настройка BSRR (GPIOB) - изначально отключаем светодиоды
    *(uint32_t *)(GPIOB_BSRR_ADDR) = (0x00080000UL | LED2_RESET | LED3_RESET | 
                                     LED4_RESET | LED5_RESET | LED6_RESET);
}
