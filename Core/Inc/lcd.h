#ifndef INC_LCD_H_
#define INC_LCD_H_

void coast_lcd_init();
void LCD_PutNibble(uint8_t nibble);
void LCD_SendData(uint8_t c);
void LCD_SendCmd(uint8_t c);
void LCD_Pulse();
void LCD_SendStr(char *str);
//commands
#define LCD_CLEAR_DISPLAY 0b00000001
//... LCD_RETURN_HOME, LCD_SECOND_LINE....

#endif /* INC_LCD_H_ */
