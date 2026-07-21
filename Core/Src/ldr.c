#include "main.h"

/**
 * @brief Reads the LDR value from ADC2 (Polling, Blocking)
 * @retval 12-bit ADC value (0-4095)
 */
uint16_t LDR_Read(ADC_HandleTypeDef *hadc)
{
    uint16_t ldr_val = 0;

    HAL_ADC_Start(hadc);
    if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK) {
        ldr_val = HAL_ADC_GetValue(hadc);
    }
    HAL_ADC_Stop(hadc);

    return ldr_val;
}

/**
 * @brief Updates the 4 PWM channels based on a brightness value
 * @param brightness: 12-bit value (0-4095)
 */
void PWM_SetBrightness(uint16_t brightness)
{
    TIM2->CCR2 = brightness;
    TIM2->CCR3 = brightness;
    TIM3->CCR1 = brightness;
    TIM3->CCR2 = brightness;
}

/**
 * @brief Master task that orchestrates reading the LDR and updating outputs
 */
void LDR_ProcessTask(ADC_HandleTypeDef *hadc)
{
    uint16_t ldr_value = LDR_Read(hadc);

    // Calculate brightness (invert LDR so darker = brighter LEDs)
    uint16_t brightness = 4095 - ldr_value;

    PWM_SetBrightness(brightness);
}

/* USER CODE END 4 */
