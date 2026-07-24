#include "schedule.h"

#include "clock.h"
#include "door_control.h"
#include "main.h"
#include "management_config.h"

static bool scheduledUnlockActive;
static bool triggeredForCurrentMinute;
static uint32_t unlockStartedAt;

void Schedule_Init(void)
{
	scheduledUnlockActive = false;
	triggeredForCurrentMinute = false;
	unlockStartedAt = 0U;
}

bool Schedule_IsUnlockActive(void)
{
	return scheduledUnlockActive;
}

void Schedule_Update(void)
{
	ClockTime_t now;
	const ManagementConfig_t *config = ManagementConfig_Get();
	bool configuredMinute;

	if (Door_IsAdministrativeOverrideActive()) {
		scheduledUnlockActive = false;
		triggeredForCurrentMinute = true;
		return;
	}

	if (scheduledUnlockActive) {
		uint32_t durationMs = (uint32_t)config->unlockDurationSeconds * 1000U;
		if ((HAL_GetTick() - unlockStartedAt) >= durationMs) {
			Door_RequestClose();
			scheduledUnlockActive = false;
		}
	}

	if (!config->scheduleEnabled || !Clock_GetTime(&now)) {
		triggeredForCurrentMinute = false;
		return;
	}

	configuredMinute = (now.hour == config->unlockHour)
			&& (now.minute == config->unlockMinute);

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
