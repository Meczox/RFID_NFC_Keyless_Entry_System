#ifndef INC_COAST_H_
#define INC_COAST_H_

#include "main.h"

#define LCD_D_Port GPIOC
#define LCD_D4_Pin GPIO_PIN_8
//...
#define LCD_E_Port GPIOD
#define LCD_E_Pin GPIO_PIN_2
//...

#define KEY_ROW_Port GPIOB
#define KEY_ROW1_Pin GPIO_PIN_11
//...

// call this at the end of MX_GPIO_Init() in main.c
void coast_gpio_init();

// call this inside while(1) in main.c
void coast_loop_body();

#endif /* INC_COAST_H_ */
