#include "init.h"


// --- Глобальные переменные ---
typedef enum {
    MODE_IDLE,
    MODE_ONE_BY_ONE,
    MODE_ALL_OFF_AFTER_7,
    MODE_TURN_OFF_ONE_BY_ONE,
    MODE_ALL_ON_AFTER_7
} MainMode;

typedef enum {
    FREQ_2HZ,
    FREQ_5HZ,
    FREQ_10HZ
} Frequency;

typedef struct {
    MainMode current_mode;
    Frequency led_frequency[6];  // Частота для каждого светодиода
    uint8_t current_led;         // Текущий светодиод для настройки (0-5)
    uint8_t led_count;           // Счетчик для поочередного включения/выключения
    uint8_t config_mode;         // Режим настройки (0-6, где 0 - не настраиваем)
    uint32_t last_led_toggle;
    uint32_t last_button_check;
    uint8_t led_states[6];       // Состояния светодиодов (0-выкл, 1-вкл)
    uint8_t button1_held;
    uint8_t button2_held;
    uint8_t button3_held;
    uint32_t button_press_time;
} SystemState;

static SystemState state = {0};

// --- Утилиты для работы со светодиодами ---
void LED_On(uint8_t led_idx) {
    switch(led_idx) {
        case 0: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED1_SET; break;
        case 1: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED2_SET; break;
        case 2: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED3_SET; break;
        case 3: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED4_SET; break;
        case 4: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED5_SET; break;
        case 5: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED6_SET; break;
    }
    state.led_states[led_idx] = 1;
}

void LED_Off(uint8_t led_idx) {
    switch(led_idx) {
        case 0: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED1_RESET; break;
        case 1: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED2_RESET; break;
        case 2: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED3_RESET; break;
        case 3: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED4_RESET; break;
        case 4: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED5_RESET; break;
        case 5: *(uint32_t*)(GPIOB_BSRR_ADDR) = LED6_RESET; break;
    }
    state.led_states[led_idx] = 0;
}

void All_LEDs_Off(void) {
    for(uint8_t i = 0; i < 6; i++) {
        LED_Off(i);
    }
}

void All_LEDs_On(void) {
    for(uint8_t i = 0; i < 6; i++) {
        LED_On(i);
    }
}

// --- Обработка кнопок с антидребезгом ---
uint8_t CheckButton(uint8_t button_mask, uint8_t* held_flag, uint32_t* press_time) {
    uint32_t current_time = Millis();
    
    if(button_mask) {  // Кнопка нажата
        if(!*held_flag) {
            *held_flag = 1;
            *press_time = current_time;
            return 0;  // Еще не готово к обработке (ждем антидребезг)
        } else if(current_time - *press_time > DEBOUNCE_TIME) {
            return 1;  // Кнопка нажата и прошло время антидребезга
        }
    } else {  // Кнопка отпущена
        if(*held_flag && current_time - *press_time > DEBOUNCE_TIME) {
            *held_flag = 0;
            // Проверяем, было ли это длинное нажатие
            if(current_time - *press_time > LONG_PRESS_TIME) {
                return 3;  // Длинное нажатие
            }
            return 2;  // Короткое нажатие
        }
        *held_flag = 0;
    }
    return 0;  // Ничего не произошло
}

// --- Обновление мигания светодиодов ---
void UpdateLEDs(void) {
    uint32_t current_time = Millis();
    
    if(current_time - state.last_led_toggle < 50) return;  // Обновляем не чаще 20 Гц
    
    state.last_led_toggle = current_time;
    
    for(uint8_t i = 0; i < 6; i++) {
        if(state.led_states[i]) {  // Если светодиод должен гореть
            uint32_t period;
            switch(state.led_frequency[i]) {
                case FREQ_2HZ: period = 250; break;   // 500 мс период = 2 Гц
                case FREQ_5HZ: period = 100; break;   // 200 мс период = 5 Гц
                case FREQ_10HZ: period = 50; break;   // 100 мс период = 10 Гц
                default: period = 250; break;
            }
            
            static uint32_t last_toggle[6] = {0};
            if(current_time - last_toggle[i] > period) {
                // Переключаем светодиод
                if(*(uint32_t*)GPIOB_BSRR_ADDR & (1 << (i + 3))) {
                    LED_Off(i);
                } else {
                    LED_On(i);
                }
                last_toggle[i] = current_time;
            }
        }
    }
}

// --- Инициализация системы ---
void System_Init(void) {
    state.current_mode = MODE_IDLE;
    state.led_count = 0;
    state.config_mode = 0;
    state.last_led_toggle = 0;
    state.last_button_check = 0;
    
    for(uint8_t i = 0; i < 6; i++) {
        state.led_frequency[i] = FREQ_2HZ;  // По умолчанию 2 Гц
        state.led_states[i] = 0;
    }
    
    All_LEDs_Off();
}

// --- Основная программа ---
int main(void) {
    GPIO_Init();
    System_Init();
    
    while(1) {
        UpdateMillis();  // Обновляем счетчик времени
        
        // Обновляем мигание светодиодов
        UpdateLEDs();
        
        // Проверяем кнопки
        uint8_t btn1 = CheckButton(BUTTON1_PRESSED, &state.button1_held, &state.button_press_time);
        uint8_t btn2 = CheckButton(BUTTON2_PRESSED, &state.button2_held, &state.button_press_time);
        uint8_t btn3 = CheckButton(BUTTON3_PRESSED, &state.button3_held, &state.button_press_time);
        
        // Обработка кнопки 1 (режим поочередного включения)
        if(btn1 == 2) {  // Короткое нажатие
            if(state.current_mode != MODE_ONE_BY_ONE && state.current_mode != MODE_ALL_OFF_AFTER_7) {
                state.current_mode = MODE_ONE_BY_ONE;
                state.led_count = 0;
                All_LEDs_Off();
            } else {
                state.led_count++;
                if(state.led_count < 6) {
                    LED_On(state.led_count);
                } else if(state.led_count == 6) {
                    All_LEDs_Off();
                    state.current_mode = MODE_ALL_OFF_AFTER_7;
                } else {
                    state.led_count = 0;
                    state.current_mode = MODE_IDLE;
                }
            }
        }
        
        // Обработка кнопки 2 (режим поочередного выключения)
        if(btn2 == 2) {  // Короткое нажатие
            if(state.current_mode != MODE_TURN_OFF_ONE_BY_ONE && state.current_mode != MODE_ALL_ON_AFTER_7) {
                state.current_mode = MODE_TURN_OFF_ONE_BY_ONE;
                state.led_count = 0;
                All_LEDs_On();
            } else {
                state.led_count++;
                if(state.led_count < 6) {
                    LED_Off(state.led_count);
                } else if(state.led_count == 6) {
                    All_LEDs_On();
                    state.current_mode = MODE_ALL_ON_AFTER_7;
                } else {
                    state.led_count = 0;
                    state.current_mode = MODE_IDLE;
                }
            }
        }
        
        // Обработка кнопки 3 (настройка частоты)
        if(btn3 == 2) {  // Короткое нажатие
            if(state.config_mode > 0) {  // Если находимся в режиме настройки
                uint8_t led_idx = state.config_mode - 1;
                // Переключаем частоту: 2Гц -> 5Гц -> 10Гц -> 2Гц
                state.led_frequency[led_idx] = (state.led_frequency[led_idx] + 1) % 3;
                
                // Мигаем светодиодом для индикации изменения
                LED_Off(led_idx);
                Delay(100);
                LED_On(led_idx);
                Delay(100);
                LED_Off(led_idx);
            }
        }
        
        if(btn3 == 3) {  // Длинное нажатие
            state.config_mode = (state.config_mode + 1) % 7;
            
            // Индикация текущего режима настройки
            if(state.config_mode > 0) {
                uint8_t led_idx = state.config_mode - 1;
                All_LEDs_Off();
                for(uint8_t i = 0; i < 3; i++) {
                    LED_On(led_idx);
                    Delay(200);
                    LED_Off(led_idx);
                    Delay(200);
                }
            }
        }
        
        // Задержка для стабильности
        //for(volatile int i = 0; i < 1000; i++);
    }
    
    return 0;
}















// // Глобальные переменные
// volatile uint8_t button_mode = 0; // 0 - нормальный режим, 1 - измененный
// volatile uint8_t animation_active = 0; 
// volatile uint8_t state_flag = 0; // текущее состояние светодиодов

// // Функция последовательного включения светодиодов
// void LEDs_Sequence_Set(void)
// {
//     state_flag = 1;
//     animation_active = 1;

//     //*(uint32_t *)GPIOB_BSRR_ADDR = LED1_SET;
//     *(uint32_t *)(0x40020400UL + 0x18UL) = 0x00000008UL;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED2_SET;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED3_SET;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED4_SET;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED5_SET;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED6_SET;
//     Delay(100);

//     animation_active = 0;
// }

// // Функция последовательного выключения светодиодов
// void LEDs_Sequence_Reset(void)
// {
//     state_flag = 0;
//     animation_active = 1;

//     //*(uint32_t *)GPIOB_BSRR_ADDR = LED1_RESET;
//     *(uint32_t *)(0x40020400UL + 0x18UL) = 0x00080000UL;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED2_RESET;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED3_RESET;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED4_RESET;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED5_RESET;
//     Delay(100);
//     *(uint32_t *)GPIOB_BSRR_ADDR = LED6_RESET;
//     Delay(100);

//     animation_active = 0;
// }

// // Функция мигания всех светодиодов (индикация смены режима)
// void Mode_Change(void)
// {
//     if (state_flag == 0)
//     {
//     for (uint8_t i = 0; i < 3; i++)
//     {
//         // Включаем все
//         *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_SET | LED2_SET | LED3_SET |
//                                         LED4_SET | LED5_SET | LED6_SET);
//         Delay(100);
//         // Выключаем все
//         *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_RESET | LED2_RESET | LED3_RESET |
//                                         LED4_RESET | LED5_RESET | LED6_RESET);
//         Delay(100);
//     }
//     }
//     else
//     {
//     for (uint8_t i = 0; i < 3; i++)
//     {
//         // Включаем все
//         *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_SET | LED2_SET | LED3_SET |
//                                         LED4_SET | LED5_SET | LED6_SET);
//         Delay(100);
//         // Выключаем все
//         *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_RESET | LED2_RESET | LED3_RESET |
//                                         LED4_RESET | LED5_RESET | LED6_RESET);
//         Delay(100);
//     }
//     *(uint32_t *)GPIOB_BSRR_ADDR = (LED1_SET | LED2_SET | LED3_SET |
//                                         LED4_SET | LED5_SET | LED6_SET);
//     }
    
// }

// int main(void)
// {
//     GPIO_Init();

//     while (1)
//     {
//         // Обработка кнопки 1
//         if (BUTTON1_PRESSED && !animation_active)
//         {
//             Delay(50); // Антидребезг
//             if (BUTTON1_PRESSED)
//             {
//                 if (button_mode == 0)
//                 {
//                     LEDs_Sequence_Set();
//                 }
//                 else
//                 {
//                     LEDs_Sequence_Reset();
//                 }
//                 // Ждем отпускания кнопки
//                 while (BUTTON1_PRESSED)
//                 {
//                 }
//             }
//         }

//         // Обработка кнопки 2
//         if (BUTTON2_PRESSED && !animation_active)
//         {
//             Delay(50); // Антидребезг
//             if (BUTTON2_PRESSED)
//             {
//                 if (button_mode == 0)
//                 {
//                     LEDs_Sequence_Reset();
//                 }
//                 else
//                 {
//                     LEDs_Sequence_Set();
//                 }
//                 // Ждем отпускания кнопки
//                 while (BUTTON2_PRESSED)
//                 {
//                 }
//             }
//         }

//         // Обработка кнопки 3
//         if (BUTTON3_PRESSED && !animation_active)
//         {
//             Delay(50); // Антидребезг
//             if (BUTTON3_PRESSED)
//             {
//                 button_mode = !button_mode;
//                 Mode_Change();
//                 // Ждем отпускания кнопки
//                 while (BUTTON3_PRESSED)
//                 {
//                 }
//             }
//         }
//     }
// }
