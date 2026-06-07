#include "coast.h"

#include "main.h"
#include "debounce.h"

uint8_t scan_keypad(){

	uint8_t ret = 0;
	uint8_t key_map[4][4] =
		   {{'1', '2', '3', 'A'},
	        {'4', '5', '6', 'B'},
	        {'7', '8', '9', 'C'},
	        {'*', '0', '#', 'D'}};

	uint16_t row_pins[4] = {ROW1_Pin, ROW2_Pin, ROW3_Pin, ROW4_Pin};
	GPIO_TypeDef* row_ports[4] = {ROW1_GPIO_Port, ROW2_GPIO_Port, ROW3_GPIO_Port, ROW4_GPIO_Port};

	uint16_t col_pins[4] = {COL1_Pin, COL2_Pin, COL3_Pin, COL4_Pin};
	GPIO_TypeDef* col_ports[4] = {COL1_GPIO_Port, COL2_GPIO_Port, COL3_GPIO_Port, COL4_GPIO_Port};

	for (int c = 0; c < 4; c++) {
		HAL_GPIO_WritePin(col_ports[c], col_pins[c], 1);

		for (int r = 0; r < 4; r++) {
			if (HAL_GPIO_ReadPin(row_ports[r], row_pins[r]) == 1) {
				isButtonPressed(row_ports[r], row_pins[r]);

				ret = key_map[r][c];
			}
		}
		HAL_GPIO_WritePin(col_ports[c], col_pins[c], 0);
	}

	return ret;
}
