#ifndef INIT_H
#define INIT_H

#include <stdint.h>

// --- Базовые адреса ---
#define RCC_BASE   0x40023800UL
#define GPIOC_BASE 0x40020800UL


// --- Смещения регистров GPIOC ---
#define GPIOC_MODER_OFFSET 0x00UL  
#define GPIOC_OTYPER_OFFSET 0x04UL 
#define GPIOC_PUPDR_OFFSET 0x0CUL  
#define GPIOC_BSRR_OFFSET 0x18UL   

#define RCC_AHB1ENR_OFFSET 0x30UL

#define RCC_AHB1ENR_ADDR (RCC_BASE + RCC_AHB1ENR_OFFSET) 

#define RCC_GPIOC_EN 0x04UL

#define GPIOC_MODER_OUTPUT 0x4000000UL 
#define GPIOC_MODER_CLEAR  0x0CUL 

#define PC13_RESET 0x20000000UL

void GPIO_Init(void);

#endif

