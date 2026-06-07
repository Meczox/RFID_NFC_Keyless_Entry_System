#include "main.h"
#include "coast.h"
#include "lcd.h"
#include "keypad.h"

void coast_gpio_keypad_init(){

}

void coast_gpio_lcd_init(){


}

void coast_gpio_init(){
	coast_gpio_lcd_init();
	coast_gpio_keypad_init();
}

void coast_loop_body(){

	LCD_SendStr("HELLO");

	while(1){
		uint8_t c = scan_keypad();
		if(c){
			LCD_SendData(c);
		}
	}

}
