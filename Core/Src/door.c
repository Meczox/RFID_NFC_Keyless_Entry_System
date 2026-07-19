#include "door.h"

#define COIL_A_GPIO_Port   GPIOC
#define COIL_A_Pin         GPIO_PIN_5

#define COIL_B_GPIO_Port   GPIOA
#define COIL_B_Pin         GPIO_PIN_12

#define COIL_C_GPIO_Port   GPIOC
#define COIL_C_Pin         GPIO_PIN_7

#define COIL_D_GPIO_Port   GPIOB
#define COIL_D_Pin         GPIO_PIN_2

#define STEP_DELAY_MS      3U
#define OPEN_STEP_COUNT    120U
#define CLOSE_STEP_COUNT   120U

static DoorState_t g_door_state = DOOR_CLOSED;
static uint8_t g_phase = 0;

static void motor_all_off(void)
{
    HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, GPIO_PIN_RESET);
}

static void motor_apply_phase(uint8_t phase)
{
    motor_all_off();

    switch (phase & 0x03U) {
    case 0:
        HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, GPIO_PIN_SET);
        break;
    case 1:
        HAL_GPIO_WritePin(COIL_B_GPIO_Port, COIL_B_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, GPIO_PIN_SET);
        break;
    case 2:
        HAL_GPIO_WritePin(COIL_C_GPIO_Port, COIL_C_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, GPIO_PIN_SET);
        break;
    case 3:
        HAL_GPIO_WritePin(COIL_D_GPIO_Port, COIL_D_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(COIL_A_GPIO_Port, COIL_A_Pin, GPIO_PIN_SET);
        break;
    }
}

static void motor_step_forward(void)
{
    g_phase = (uint8_t)((g_phase + 1U) & 0x03U);
    motor_apply_phase(g_phase);
}

static void motor_step_reverse(void)
{
    g_phase = (uint8_t)((g_phase + 3U) & 0x03U);
    motor_apply_phase(g_phase);
}

void Door_Init(void)
{
    motor_all_off();
    g_door_state = DOOR_CLOSED;
    g_phase = 0;
}

void Door_OpenForEntry(void)
{
    g_door_state = DOOR_MOVING_ENTRY;

    for (uint16_t i = 0; i < OPEN_STEP_COUNT; i++) {
        motor_step_forward();
        HAL_Delay(STEP_DELAY_MS);
    }

    motor_all_off();
    g_door_state = DOOR_OPEN;
}

void Door_OpenForExit(void)
{
    g_door_state = DOOR_MOVING_EXIT;

    for (uint16_t i = 0; i < OPEN_STEP_COUNT; i++) {
        motor_step_reverse();
        HAL_Delay(STEP_DELAY_MS);
    }

    motor_all_off();
    g_door_state = DOOR_OPEN;
}

void Door_Close(void)
{
    if (g_door_state == DOOR_CLOSED) {
        return;
    }

    for (uint16_t i = 0; i < CLOSE_STEP_COUNT; i++) {
        motor_step_reverse();
        HAL_Delay(STEP_DELAY_MS);
    }

    motor_all_off();
    g_door_state = DOOR_CLOSED;
}

DoorState_t Door_GetState(void)
{
    return g_door_state;
}

bool Door_IsOpen(void)
{
    return (g_door_state == DOOR_OPEN);
}

bool Door_IsClosed(void)
{
    return (g_door_state == DOOR_CLOSED);
}
