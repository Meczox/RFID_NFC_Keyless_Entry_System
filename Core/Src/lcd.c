#include "main.h"
#include "coast.h"
#include "lcd.h"

#define LCD_CMD_CLEAR          0b00000001
#define LCD_CMD_ENTRY_MODE     0b00000110
#define LCD_CMD_DISPLAY_OFF    0b00001000
#define LCD_CMD_DISPLAY_ON     0b00001111
#define LCD_CMD_FUNCTION_SET   0b00101100
#define LCD_CMD_LINE_2         0b11000000

void coast_lcd_init(){
	// 1. wait for enough time to stabilise
	HAL_Delay(50);
	// 2. send command 0011 (function set) and wait for >=4.1 ms (enough wait inside the pulse)
	LCD_PutNibble(0b0011);
	HAL_Delay(5);
	// 3. send command 0011 (function set) again and wait for >=100 us
	LCD_PutNibble(0b0011);
	HAL_Delay(1);
	// 4. send command 0011 (function set) again
	LCD_PutNibble(0b0011);
	// 5. send command 0010 to set to 4-bit bus mode
	LCD_PutNibble(0b0010);
	// 6. send command 0010 1100 (function set: 4-bit mode, 2-lines, 5x8 font)
	LCD_SendCmd(LCD_CMD_FUNCTION_SET);
	// 7. Send command 0000 1000 to display ON/OFF
	LCD_SendCmd(LCD_CMD_DISPLAY_OFF);
	// 8. Send command to clear the display
	LCD_SendCmd(LCD_CMD_CLEAR);
	// 9. Send command set entry mode (increment cursor, no display shift)
	LCD_SendCmd(LCD_CMD_ENTRY_MODE);
	// 10. send command 0000 1111 to display on, cursor on, blink on
	LCD_SendCmd(LCD_CMD_DISPLAY_ON);
}

void LCD_Pulse(){
	HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, 1);
	HAL_Delay(2);
	HAL_GPIO_WritePin(LCD_CE_GPIO_Port, LCD_CE_Pin, 0);
	HAL_Delay(2);
}

void LCD_PutNibble(uint8_t nibble){
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (nibble >> 0) & 1);
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D5_Pin, (nibble >> 1) & 1);
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D6_Pin, (nibble >> 2) & 1);
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D7_Pin, (nibble >> 3) & 1);
	LCD_Pulse();
}

void LCD_SendCmd(uint8_t c){
	HAL_GPIO_WritePin(LCD_CRS_GPIO_Port, LCD_CRS_Pin, 0);
	HAL_GPIO_WritePin(LCD_CRW_GPIO_Port, LCD_CRW_Pin, 0);

	LCD_PutNibble(c >> 4);
	LCD_PutNibble(c & 0b00001111);
}

// for 1 letter at a time
void LCD_SendData(uint8_t c){
	HAL_GPIO_WritePin(LCD_CRS_GPIO_Port, LCD_CRS_Pin, 1);
	HAL_GPIO_WritePin(LCD_CRW_GPIO_Port, LCD_CRW_Pin, 0);

	LCD_PutNibble(c >> 4);
	LCD_PutNibble(c & 0b00001111);
}

// for a string
void LCD_SendStr(char *str){
	while(*str != '\0') {
		LCD_SendData(*str);
		str++;
	}
}
