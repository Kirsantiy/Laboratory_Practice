#include "init.h"

// Глобальные переменные
volatile uint8_t button_mode = 0; // 0 - нормальный режим, 1 - измененный
volatile uint8_t animation_active = 0; 
volatile uint8_t state_flag = 0; // текущее состояние светодиодов

// Функция последовательного включения светодиодов
void LEDs_Sequence_Set(void)
{
    state_flag = 1;
    animation_active = 1;

    //*(uint32_t *)GPIOB_BSRR_ADDR = LED1_SET;
    *(uint32_t *)(0x40020400UL + 0x18UL) = 0x00000008UL;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED2_SET;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED3_SET;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED4_SET;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED5_SET;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED6_SET;
    Delay(100);

    animation_active = 0;
}

// Функция последовательного выключения светодиодов
void LEDs_Sequence_Reset(void)
{
    state_flag = 0;
    animation_active = 1;

    //*(uint32_t *)GPIOB_BSRR_ADDR = LED1_RESET;
    *(uint32_t *)(0x40020400UL + 0x18UL) = 0x00080000UL;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED2_RESET;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED3_RESET;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED4_RESET;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED5_RESET;
    Delay(100);
    *(uint32_t *)GPIOB_BSRR_ADDR = LED6_RESET;
    Delay(100);

    animation_active = 0;
}

// Функция мигания всех светодиодов (индикация смены режима)
void Mode_Change(void)
{
    if (state_flag == 0)
    {
    for (uint8_t i = 0; i < 3; i++)
    {
        // Включаем все
        *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_SET | LED2_SET | LED3_SET |
                                        LED4_SET | LED5_SET | LED6_SET);
        Delay(100);
        // Выключаем все
        *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_RESET | LED2_RESET | LED3_RESET |
                                        LED4_RESET | LED5_RESET | LED6_RESET);
        Delay(100);
    }
    }
    else
    {
    for (uint8_t i = 0; i < 3; i++)
    {
        // Включаем все
        *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_SET | LED2_SET | LED3_SET |
                                        LED4_SET | LED5_SET | LED6_SET);
        Delay(100);
        // Выключаем все
        *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_RESET | LED2_RESET | LED3_RESET |
                                        LED4_RESET | LED5_RESET | LED6_RESET);
        Delay(100);
    }
    *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_SET | LED2_SET | LED3_SET |
                                        LED4_SET | LED5_SET | LED6_SET);
    }
    
}

int main(void)
{
    GPIO_Init();

    while (1)
    {
        // Обработка кнопки 1
        if (BUTTON1_PRESSED && !animation_active)
        {
            Delay(50); // Антидребезг
            if (BUTTON1_PRESSED)
            {
                if (button_mode == 0)
                {
                    LEDs_Sequence_Set();
                }
                else
                {
                    LEDs_Sequence_Reset();
                }
                // Ждем отпускания кнопки
                while (BUTTON1_PRESSED)
                {
                }
            }
        }

        // Обработка кнопки 2
        if (BUTTON2_PRESSED && !animation_active)
        {
            Delay(50); // Антидребезг
            if (BUTTON2_PRESSED)
            {
                if (button_mode == 0)
                {
                    LEDs_Sequence_Reset();
                }
                else
                {
                    LEDs_Sequence_Set();
                }
                // Ждем отпускания кнопки
                while (BUTTON2_PRESSED)
                {
                }
            }
        }

        // Обработка кнопки 3
        if (BUTTON3_PRESSED && !animation_active)
        {
            Delay(50); // Антидребезг
            if (BUTTON3_PRESSED)
            {
                button_mode = !button_mode;
                Mode_Change();
                // Ждем отпускания кнопки
                while (BUTTON3_PRESSED)
                {
                }
            }
        }
    }
}
