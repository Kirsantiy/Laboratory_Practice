#ifndef INIT_H
#define INIT_H

#include <stdint.h>

// По умолчанию, если ничего не менять, то в качестве тактирующего элемента стоит HSI (16 МГц)

// --- Базовые адреса ---
#define RCC_BASE   0x40023800UL
#define GPIOA_BASE 0x40020000UL
#define GPIOB_BASE 0x40020400UL

// --- Смещения регистров GPIO ---
#define GPIO_MODER_OFFSET  0x00UL
#define GPIO_OTYPER_OFFSET 0x04UL
#define GPIO_PUPDR_OFFSET  0x0CUL
#define GPIO_IDR_OFFSET    0x10UL
#define GPIO_BSRR_OFFSET   0x18UL

#define RCC_AHB1ENR_OFFSET 0x30UL

// --- Адреса регистров ---
#define RCC_AHB1ENR_ADDR (RCC_BASE + RCC_AHB1ENR_OFFSET) //
#define GPIOA_MODER_ADDR (GPIOA_BASE + GPIO_MODER_OFFSET) //
#define GPIOA_OTYPER_ADDR (GPIOA_BASE + GPIO_OTYPER_OFFSET) //
#define GPIOA_PUPDR_ADDR (GPIOA_BASE + GPIO_PUPDR_OFFSET) //
#define GPIOA_IDR_ADDR (GPIOA_BASE + GPIO_IDR_OFFSET)
#define GPIOB_MODER_ADDR (GPIOB_BASE + GPIO_MODER_OFFSET) //
#define GPIOB_OTYPER_ADDR (GPIOB_BASE + GPIO_OTYPER_OFFSET) //
#define GPIOB_PUPDR_ADDR (GPIOB_BASE + GPIO_PUPDR_OFFSET) //
#define GPIOB_BSRR_ADDR (GPIOB_BASE + GPIO_BSRR_OFFSET)

// --- Битовые маски для RCC (GPIOA, GPIOB) ---
#define RCC_GPIOA_EN 0x01
#define RCC_GPIOB_EN 0x02

#define GPIOA_MODER_INPUT 0x00000000UL

#define GPIOB_MODER_OUTPUT 0x00015540UL
#define GPIOB_MODER_CLEAR  0x00000000UL

#define GPIOA_PUPD 0x15
#define GPIOB_PUPD 0x00000000UL

// --- Маски для кнопок (PA0-PA2) ---
#define BUTTON1_MASK    0x00000001UL  // PA0
#define BUTTON2_MASK    0x00000002UL  // PA1  
#define BUTTON3_MASK    0x00000004UL  // PA2

// --- Маски для светодиодов (PB3-PB8) ---
#define LED1_MASK       0x00000008UL  // PB3
#define LED2_MASK       0x00000010UL  // PB4
#define LED3_MASK       0x00000020UL  // PB5
#define LED4_MASK       0x00000040UL  // PB6
#define LED5_MASK       0x00000080UL  // PB7
#define LED6_MASK       0x00000100UL  // PB8 

// --- Маски для BSRR ---
#define LED1_SET    0x00000008UL  // PB3 SET
#define LED1_RESET  0x00080000UL  // PB3 RESET  
#define LED2_SET    0x00000010UL  // PB4 SET
#define LED2_RESET  0x00100000UL  // PB4 RESET
#define LED3_SET    0x00000020UL  // PB5 SET
#define LED3_RESET  0x00200000UL  // PB5 RESET
#define LED4_SET    0x00000040UL  // PB6 SET
#define LED4_RESET  0x00400000UL  // PB6 RESET
#define LED5_SET    0x00000080UL  // PB7 SET
#define LED5_RESET  0x00800000UL  // PB7 RESET
#define LED6_SET    0x00000100UL  // PB8 SET
#define LED6_RESET  0x01000000UL  // PB8 RESET

// --- Обработка нажатия кнопок ---
#define BUTTON1_PRESSED !(*(uint32_t*)GPIOA_IDR_ADDR & BUTTON1_MASK)
#define BUTTON2_PRESSED !(*(uint32_t*)GPIOA_IDR_ADDR & BUTTON2_MASK)
#define BUTTON3_PRESSED !(*(uint32_t*)GPIOA_IDR_ADDR & BUTTON3_MASK)

void GPIO_Init(void);
void Delay(uint32_t ms);

#endif
