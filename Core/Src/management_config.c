#include "management_config.h"

#define DEFAULT_UNLOCK_HOUR              9U
#define DEFAULT_UNLOCK_MINUTE            0U
#define DEFAULT_UNLOCK_DURATION_SECONDS 30U
#define MAX_UNLOCK_DURATION_SECONDS     999U

static ManagementConfig_t currentConfig;
static bool adminModeActive;

// unlock info
void ManagementConfig_Init(void)
{
	currentConfig.unlockHour = DEFAULT_UNLOCK_HOUR;
	currentConfig.unlockMinute = DEFAULT_UNLOCK_MINUTE;
	currentConfig.unlockDurationSeconds = DEFAULT_UNLOCK_DURATION_SECONDS;
	currentConfig.scheduleEnabled = false;
	adminModeActive = false;
}

const ManagementConfig_t *ManagementConfig_Get(void)
{
	return &currentConfig;
}

bool ManagementConfig_SetUnlockTime(uint8_t hour, uint8_t minute)
{
	if (hour > 23U || minute > 59U) {
		return false;
	}

	currentConfig.unlockHour = hour;
	currentConfig.unlockMinute = minute;
	return true;
}

bool ManagementConfig_SetUnlockDuration(uint16_t durationSeconds)
{
	if (durationSeconds == 0U || durationSeconds > MAX_UNLOCK_DURATION_SECONDS) {
		return false;
	}

	currentConfig.unlockDurationSeconds = durationSeconds;
	return true;
}

void ManagementConfig_SetScheduleEnabled(bool enabled)
{
	currentConfig.scheduleEnabled = enabled;
}

void ManagementConfig_EnterAdminMode(void)
{
	adminModeActive = true;
}

void ManagementConfig_ExitAdminMode(void)
{
	adminModeActive = false;
}

bool ManagementConfig_IsAdminMode(void)
{
	return adminModeActive;
}
