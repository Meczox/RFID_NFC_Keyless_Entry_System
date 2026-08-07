#include "schedule.h"

#include "clock.h"
#include "door_control.h"
#include "main.h"
#include "management_config.h"

static bool scheduledUnlockActive;
static bool triggeredForCurrentMinute;
static uint32_t unlockStartedAt;
static bool specificDateOpened;

static int compare_dates(const ClockDate_t *left, const ManagementConfig_t *config)
{
	if (left->year != config->unlockYear) {
		return (left->year < config->unlockYear) ? -1 : 1;
	}
	if (left->month != config->unlockMonth) {
		return (left->month < config->unlockMonth) ? -1 : 1;
	}
	if (left->day != config->unlockDay) {
		return (left->day < config->unlockDay) ? -1 : 1;
	}
	return 0;
}

void Schedule_Init(void)
{
	scheduledUnlockActive = false;
	triggeredForCurrentMinute = false;
	unlockStartedAt = 0U;
	specificDateOpened = false;
}

bool Schedule_IsUnlockActive(void)
{
	return scheduledUnlockActive;
}

void Schedule_Update(void)
{
	ClockDateTime_t now;
	const ManagementConfig_t *config = ManagementConfig_Get();
	bool configuredMinute;
	int dateRelation;

	if (Door_IsAdministrativeOverrideActive()) {
		scheduledUnlockActive = false;
		triggeredForCurrentMinute = true;
		specificDateOpened = false;
		return;
	}

	if (config->useSpecificDate) {
		if (!config->scheduleEnabled || !Clock_GetDateTime(&now)) {
			triggeredForCurrentMinute = false;
			return;
		}

		dateRelation = compare_dates(&now.date, config);
		if (dateRelation < 0) {
			triggeredForCurrentMinute = false;
			specificDateOpened = false;
			return;
		}

		if (dateRelation > 0) {
			if (scheduledUnlockActive && Door_IsDoorwayClear()) {
				Door_RequestClose();
				scheduledUnlockActive = false;
			}
			triggeredForCurrentMinute = false;
			specificDateOpened = true;
			return;
		}

		configuredMinute = (now.time.hour == config->specificDateOpenHour)
				&& (now.time.minute == config->specificDateOpenMinute);
		if (!specificDateOpened && configuredMinute && !triggeredForCurrentMinute) {
			Door_RequestScheduledUnlock();
			unlockStartedAt = HAL_GetTick();
			scheduledUnlockActive = true;
			specificDateOpened = true;
			triggeredForCurrentMinute = true;
			return;
		}

		if (scheduledUnlockActive) {
			uint32_t durationMs =
					(uint32_t)config->specificDateDurationSeconds * 1000U;
			if ((HAL_GetTick() - unlockStartedAt) >= durationMs
					&& Door_IsDoorwayClear()) {
				Door_RequestClose();
				scheduledUnlockActive = false;
			}
		}

		if (now.time.hour != config->specificDateOpenHour
				|| now.time.minute != config->specificDateOpenMinute) {
			triggeredForCurrentMinute = false;
		}
		return;
	}

	if (scheduledUnlockActive) {
		uint32_t durationMs = (uint32_t)config->unlockDurationSeconds * 1000U;
		if ((HAL_GetTick() - unlockStartedAt) >= durationMs
				&& Door_IsDoorwayClear()) {
			Door_RequestClose();
			scheduledUnlockActive = false;
		}
	}

	if (!config->scheduleEnabled || !Clock_GetDateTime(&now)) {
		triggeredForCurrentMinute = false;
		return;
	}

	configuredMinute = (now.time.hour == config->unlockHour)
			&& (now.time.minute == config->unlockMinute);

	if (!configuredMinute) {
		triggeredForCurrentMinute = false;
		return;
	}

	if (!triggeredForCurrentMinute && !scheduledUnlockActive) {
		Door_RequestScheduledUnlock();
		unlockStartedAt = HAL_GetTick();
		scheduledUnlockActive = true;
		triggeredForCurrentMinute = true;
	}
}
