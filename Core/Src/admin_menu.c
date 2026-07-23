#include "admin_menu.h"

#include "door_control.h"
#include "lcd.h"
#include "main.h"
#include "management_config.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static AdminMenuState_t menuState;

// keyboard input
static char inputBuffer[5];
static uint8_t inputLength;
static uint32_t feedbackUntil;

#define FEEDBACK_DURATION_MS 1000U

static void show_current_config(void);

void AdminMenu_Init(void)
{
    menuState = ADMIN_MENU_INACTIVE;
    inputLength = 0;
    feedbackUntil = 0U;
    memset(inputBuffer, 0, sizeof(inputBuffer));
}

// only use in this file
static void clear_input(void)
{
    inputLength = 0;
    memset(inputBuffer, 0, sizeof(inputBuffer));
}

static void show_main_menu(void)
{
    LCD_SendCmd(LCD_CLEAR_DISPLAY);
    LCD_SendStr("A:TIME B:DUR");

    LCD_SendCmd(LCD_SECOND_LINE);
    LCD_SendStr("C:CFG D:X #:DOOR");
}

static void show_time_input(void)
{
	char line[17];

	snprintf(
		line,
		sizeof(line),
		"%c%c:%c%c #=SAVE",
		inputLength > 0U ? inputBuffer[0] : '_',
		inputLength > 1U ? inputBuffer[1] : '_',
		inputLength > 2U ? inputBuffer[2] : '_',
		inputLength > 3U ? inputBuffer[3] : '_'
	);

    LCD_SendCmd(LCD_CLEAR_DISPLAY);
    LCD_SendStr("SET TIME HHMM");

    LCD_SendCmd(LCD_SECOND_LINE);
    LCD_SendStr(line);
}

static void show_duration_input(void)
{
	char line[17];

	snprintf(line, sizeof(line), "%-3s SEC #=SAVE", inputBuffer);

    LCD_SendCmd(LCD_CLEAR_DISPLAY);
    LCD_SendStr("DURATION 1-999");

    LCD_SendCmd(LCD_SECOND_LINE);
    LCD_SendStr(line);
}

static void show_feedback(const char *message)
{
	LCD_SendCmd(LCD_CLEAR_DISPLAY);
	LCD_SendStr(message);

	menuState = ADMIN_MENU_FEEDBACK;
	feedbackUntil = HAL_GetTick() + FEEDBACK_DURATION_MS;
}

// can be used everywhere
void AdminMenu_Enter(void)
{
    clear_input();
    menuState = ADMIN_MENU_MAIN;
    show_main_menu();
}

static void handle_main_menu(uint8_t key)
{
    switch (key) {
    case 'A':
        clear_input();
        menuState = ADMIN_MENU_SET_TIME;
        show_time_input();
        break;

    case 'B':
        clear_input();
        menuState = ADMIN_MENU_SET_DURATION;
        show_duration_input();
        break;

    case 'C':
        menuState = ADMIN_MENU_SHOW_CONFIG;
        show_current_config();
        break;

    case 'D':
        if (Door_IsAdministrativeOverrideActive() && Door_IsOpen()) {
                show_feedback("CLOSE DOOR FIRST");
                break;
            }

            ManagementConfig_ExitAdminMode();
            menuState = ADMIN_MENU_INACTIVE;

            LCD_SendCmd(LCD_CLEAR_DISPLAY);
            LCD_SendStr("ADMIN EXIT");
            break;
    case '#': {
        DoorAdminOverrideResult_t result = Door_ToggleAdministrativeOverride();

            if (result == DOOR_ADMIN_OVERRIDE_OPENED) {
                show_feedback("OVERRIDE OPEN");
            } else if (result == DOOR_ADMIN_OVERRIDE_CLOSED) {
                show_feedback("DOOR CLOSED");
            } else {
                show_feedback("DOOR BUSY");
            }
            break;
        
    }        
    default:
        break;
    }
}

static void handle_time_input(uint8_t key)
{
    if (key >= '0' && key <= '9') {
        if (inputLength < 4) {
            inputBuffer[inputLength] = (char)key;
            inputLength++;
            inputBuffer[inputLength] = '\0';
			show_time_input();
        }
        return;
    }

    if (key == '*') {
        clear_input();
        menuState = ADMIN_MENU_MAIN;
        show_main_menu();
        return;
    }

    if (key == '#' && inputLength == 4) {
        uint8_t hour =
            (uint8_t)((inputBuffer[0] - '0') * 10 +
                      (inputBuffer[1] - '0'));

        uint8_t minute =
            (uint8_t)((inputBuffer[2] - '0') * 10 +
                      (inputBuffer[3] - '0'));

        if (ManagementConfig_SetUnlockTime(hour, minute)) {
            ManagementConfig_SetScheduleEnabled(true);
			show_feedback("TIME SAVED");
        } else {
			show_feedback("INVALID TIME");
        }

        clear_input();
    }
}

static void handle_duration_input(uint8_t key)
{
    if (key >= '0' && key <= '9') {
        if (inputLength < 3) {
            inputBuffer[inputLength] = (char)key;
            inputLength++;
            inputBuffer[inputLength] = '\0';
			show_duration_input();
        }
        return;
    }

    if (key == '*') {
        clear_input();
        menuState = ADMIN_MENU_MAIN;
        show_main_menu();
        return;
    }

    if (key == '#' && inputLength > 0) {
        uint16_t duration = 0;

        for (uint8_t i = 0; i < inputLength; i++) {
            duration =
                (uint16_t)(duration * 10U +
                           (uint16_t)(inputBuffer[i] - '0'));
        }

        if (ManagementConfig_SetUnlockDuration(duration)) {
			show_feedback("DURATION SAVED");
        } else {
			show_feedback("INVALID VALUE");
        }

        clear_input();
    }
}

static void show_current_config(void)
{
    char line[17];

    const ManagementConfig_t *config =
        ManagementConfig_Get();

    LCD_SendCmd(LCD_CLEAR_DISPLAY);

    snprintf(
        line,
        sizeof(line),
        "TIME %02u:%02u",
		(unsigned int)config->unlockHour,
		(unsigned int)config->unlockMinute
    );

    LCD_SendStr(line);

    LCD_SendCmd(LCD_SECOND_LINE);

    snprintf(
        line,
        sizeof(line),
        "DURATION %us",
		(unsigned int)config->unlockDurationSeconds
    );

    LCD_SendStr(line);
}

void AdminMenu_Update(uint8_t key)
{
	if (menuState == ADMIN_MENU_FEEDBACK) {
		if ((int32_t)(HAL_GetTick() - feedbackUntil) >= 0) {
			menuState = ADMIN_MENU_MAIN;
			show_main_menu();
		}
		return;
	}

    if (key == 0) {
        return;
    }

    switch (menuState) {
    case ADMIN_MENU_MAIN:
        handle_main_menu(key);
        break;

    case ADMIN_MENU_SET_TIME:
        handle_time_input(key);
        break;

    case ADMIN_MENU_SET_DURATION:
        handle_duration_input(key);
        break;

    case ADMIN_MENU_SHOW_CONFIG:
        if (key == '*') {
            menuState = ADMIN_MENU_MAIN;
            show_main_menu();
        } else if (key == 'D') {
            ManagementConfig_ExitAdminMode();
            menuState = ADMIN_MENU_INACTIVE;
        }
        break;

    case ADMIN_MENU_INACTIVE:
    default:
        break;
    }
}

AdminMenuState_t AdminMenu_GetState(void)
{
	return menuState;
}
