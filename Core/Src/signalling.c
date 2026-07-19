#include "signalling.h"

extern TIM_HandleTypeDef htim1;

#define LED_AUTH_GPIO_Port     GPIOB
#define LED_AUTH_Pin           GPIO_PIN_10

#define LED_ALERT1_GPIO_Port   GPIOB
#define LED_ALERT1_Pin         GPIO_PIN_4

#define LED_ALERT2_GPIO_Port   GPIOB
#define LED_ALERT2_Pin         GPIO_PIN_5

#define LED_ALERT3_GPIO_Port   GPIOB
#define LED_ALERT3_Pin         GPIO_PIN_3

#define AUTHORISED_FLASH_MS    400U
#define ADMIN_ON_MS            120U
#define ADMIN_OFF_MS           120U
#define ADMIN_FLASH_COUNT      2U

#define ALERT_ON_MS            180U
#define ALERT_OFF_MS           120U
#define ALERT_PULSE_COUNT      5U

typedef struct {
    SignalPattern_t pattern;
    uint32_t start_time;
    uint32_t last_toggle_time;
    uint8_t pulses_remaining;
    bool outputs_on;
} SignallingState_t;

static SignallingState_t g_sig;

static void led_auth_on(void)
{
    HAL_GPIO_WritePin(LED_AUTH_GPIO_Port, LED_AUTH_Pin, GPIO_PIN_SET);
}

static void led_auth_off(void)
{
    HAL_GPIO_WritePin(LED_AUTH_GPIO_Port, LED_AUTH_Pin, GPIO_PIN_RESET);
}

static void alert_leds_on(void)
{
    HAL_GPIO_WritePin(LED_ALERT1_GPIO_Port, LED_ALERT1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_ALERT2_GPIO_Port, LED_ALERT2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_ALERT3_GPIO_Port, LED_ALERT3_Pin, GPIO_PIN_SET);
}

static void alert_leds_off(void)
{
    HAL_GPIO_WritePin(LED_ALERT1_GPIO_Port, LED_ALERT1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_ALERT2_GPIO_Port, LED_ALERT2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_ALERT3_GPIO_Port, LED_ALERT3_Pin, GPIO_PIN_RESET);
}

static void buzzer_on(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

static void buzzer_off(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
}

void Signalling_Init(void)
{
    g_sig.pattern = SIGNAL_IDLE;
    g_sig.start_time = 0U;
    g_sig.last_toggle_time = 0U;
    g_sig.pulses_remaining = 0U;
    g_sig.outputs_on = false;
    Signalling_AllOff();
}

void Signalling_AllOff(void)
{
    led_auth_off();
    alert_leds_off();
    buzzer_off();

    g_sig.pattern = SIGNAL_IDLE;
    g_sig.start_time = 0U;
    g_sig.last_toggle_time = 0U;
    g_sig.pulses_remaining = 0U;
    g_sig.outputs_on = false;
}

void Indicator_Signal_Authorised(void)
{
    Signalling_AllOff();
    g_sig.pattern = SIGNAL_AUTHORISED;
    g_sig.start_time = HAL_GetTick();
    led_auth_on();
}

void Indicator_Signal_Admin(void)
{
    Signalling_AllOff();
    g_sig.pattern = SIGNAL_ADMIN;
    g_sig.start_time = HAL_GetTick();
    g_sig.last_toggle_time = g_sig.start_time;
    g_sig.pulses_remaining = ADMIN_FLASH_COUNT;
    g_sig.outputs_on = true;
    led_auth_on();
}

void Indicator_Signal_DoorOpen(void)
{
    led_auth_on();
}

void Indicator_Signal_DoorClosed(void)
{
    led_auth_off();
}

void Alarm_Trigger_Unauthorised(void)
{
    Signalling_AllOff();
    g_sig.pattern = SIGNAL_UNAUTHORISED;
    g_sig.start_time = HAL_GetTick();
    g_sig.last_toggle_time = g_sig.start_time;
    g_sig.pulses_remaining = ALERT_PULSE_COUNT;
    g_sig.outputs_on = true;
    alert_leds_on();
    buzzer_on();
}

void Alarm_Clear(void)
{
    alert_leds_off();
    buzzer_off();

    if (g_sig.pattern == SIGNAL_UNAUTHORISED) {
        g_sig.pattern = SIGNAL_IDLE;
        g_sig.pulses_remaining = 0U;
        g_sig.outputs_on = false;
    }
}

void Signalling_RunTask(void)
{
    uint32_t now = HAL_GetTick();

    switch (g_sig.pattern) {
    case SIGNAL_IDLE:
        break;

    case SIGNAL_AUTHORISED:
        if ((now - g_sig.start_time) >= AUTHORISED_FLASH_MS) {
            led_auth_off();
            g_sig.pattern = SIGNAL_IDLE;
        }
        break;

    case SIGNAL_ADMIN:
        if (g_sig.pulses_remaining == 0U) {
            led_auth_off();
            g_sig.pattern = SIGNAL_IDLE;
            break;
        }

        if (g_sig.outputs_on) {
            if ((now - g_sig.last_toggle_time) >= ADMIN_ON_MS) {
                led_auth_off();
                g_sig.outputs_on = false;
                g_sig.last_toggle_time = now;
                g_sig.pulses_remaining--;
            }
        } else {
            if ((now - g_sig.last_toggle_time) >= ADMIN_OFF_MS) {
                led_auth_on();
                g_sig.outputs_on = true;
                g_sig.last_toggle_time = now;
            }
        }
        break;

    case SIGNAL_DOOR_OPEN:
        break;

    case SIGNAL_UNAUTHORISED:
        if (g_sig.pulses_remaining == 0U) {
            alert_leds_off();
            buzzer_off();
            g_sig.pattern = SIGNAL_IDLE;
            break;
        }

        if (g_sig.outputs_on) {
            if ((now - g_sig.last_toggle_time) >= ALERT_ON_MS) {
                alert_leds_off();
                buzzer_off();
                g_sig.outputs_on = false;
                g_sig.last_toggle_time = now;
                g_sig.pulses_remaining--;
            }
        } else {
            if ((now - g_sig.last_toggle_time) >= ALERT_OFF_MS) {
                alert_leds_on();
                buzzer_on();
                g_sig.outputs_on = true;
                g_sig.last_toggle_time = now;
            }
        }
        break;

    default:
        Signalling_AllOff();
        break;
    }
}
