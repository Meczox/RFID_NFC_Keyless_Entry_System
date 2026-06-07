#include "debounce.h"

#include "debounce.h"

void isButtonPressed(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    const int confidenceThreshold = 3000;

    volatile int press_confidence = 0;
    volatile int release_confidence = 0;

    while (press_confidence < confidenceThreshold) {
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 1) {
            press_confidence++;
            release_confidence = 0;
        } else {
            press_confidence = 0;
        }
    }

    while (release_confidence < confidenceThreshold) {
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 0) {
            release_confidence++;
        } else {
            release_confidence = 0;
        }
    }
}
