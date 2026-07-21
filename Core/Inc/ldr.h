#ifndef INC_LDR_H_
#define INC_LDR_H_

uint16_t LDR_Read(ADC_HandleTypeDef *hadc);
void PWM_SetBrightness(uint16_t brightness);
void ShiftReg_SetLEDBar(uint16_t brightness);
void LDR_ProcessTask(void);

#endif
