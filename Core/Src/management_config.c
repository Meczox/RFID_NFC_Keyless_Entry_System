#include "management_config.h"

#define DEFAULT_UNLOCK_HOUR              9U
#define DEFAULT_UNLOCK_MINUTE            0U
#define DEFAULT_UNLOCK_DURATION_SECONDS 30U
#define MAX_UNLOCK_DURATION_SECONDS     999U

static ManagementConfig_t currentConfig;
static bool adminModeActive;

static bool is_leap_year(uint16_t year)
{
	return ((year % 4U) == 0U && (year % 100U) != 0U)
			|| ((year % 400U) == 0U);
}

static uint8_t days_in_month(uint8_t month, uint16_t year)
{
	static const uint8_t daysPerMonth[] = {
		31U, 28U, 31U, 30U, 31U, 30U,
		31U, 31U, 30U, 31U, 30U, 31U
	};

	if (month < 1U || month > 12U) {
		return 0U;
	}

	if (month == 2U && is_leap_year(year)) {
		return 29U;
	}

	return daysPerMonth[month - 1U];
}

// unlock info
void ManagementConfig_Init(void)
{
	currentConfig.unlockHour = DEFAULT_UNLOCK_HOUR;
	currentConfig.unlockMinute = DEFAULT_UNLOCK_MINUTE;
	currentConfig.specificDateOpenHour = DEFAULT_UNLOCK_HOUR;
	currentConfig.specificDateOpenMinute = DEFAULT_UNLOCK_MINUTE;
	currentConfig.unlockDay = 0U;
	currentConfig.unlockMonth = 0U;
	currentConfig.unlockYear = 0U;
	currentConfig.unlockDurationSeconds = DEFAULT_UNLOCK_DURATION_SECONDS;
	currentConfig.specificDateDurationSeconds = DEFAULT_UNLOCK_DURATION_SECONDS;
	currentConfig.useSpecificDate = false;
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

bool ManagementConfig_SetSpecificDateOpenTime(uint8_t hour, uint8_t minute)
{
	if (hour > 23U || minute > 59U) {
		return false;
	}

	currentConfig.specificDateOpenHour = hour;
	currentConfig.specificDateOpenMinute = minute;
	return true;
}

bool ManagementConfig_SetSpecificDateDuration(uint16_t durationSeconds)
{
	if (durationSeconds == 0U || durationSeconds > MAX_UNLOCK_DURATION_SECONDS) {
		return false;
	}

	currentConfig.specificDateDurationSeconds = durationSeconds;
	return true;
}

bool ManagementConfig_SetUnlockDate(uint8_t day, uint8_t month, uint16_t year)
{
	uint8_t maxDay;

	if (year < 2000U || year > 2099U) {
		return false;
	}

	maxDay = days_in_month(month, year);
	if (maxDay == 0U || day < 1U || day > maxDay) {
		return false;
	}

	currentConfig.unlockDay = day;
	currentConfig.unlockMonth = month;
	currentConfig.unlockYear = year;
	currentConfig.useSpecificDate = true;
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

void ManagementConfig_ClearUnlockDate(void)
{
	currentConfig.unlockDay = 0U;
	currentConfig.unlockMonth = 0U;
	currentConfig.unlockYear = 0U;
	currentConfig.useSpecificDate = false;
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
