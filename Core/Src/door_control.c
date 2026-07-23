#include "door_control.h"

#include "door.h"
#include "signalling.h"
static bool adminOverrideActive = false;
__weak void Door_RequestAuthorizedEntry(void)
{
	Indicator_Signal_Authorised();
	Door_OpenForEntry();
	Indicator_Signal_DoorOpen();
}

__weak void Door_RequestScheduledUnlock(void)
{
	Door_OpenForEntry();
	Indicator_Signal_DoorOpen();
}

__weak void Door_RequestClose(void)
{
	Door_Close();
	Indicator_Signal_DoorClosed();
}

__weak void Door_ReportUnauthorizedCredential(void)
{
	Alarm_Trigger_Unauthorised();
}

__weak bool Door_CanCloseAdministrativeOverride(void)
{
	return true;
}

bool Door_IsAdministrativeOverrideActive(void)
{
	return adminOverrideActive;
}

__weak DoorAdminOverrideResult_t Door_ToggleAdministrativeOverride(void)
{
	if (!adminOverrideActive) {
		Indicator_Signal_Authorised();
		if (!Door_IsOpen()) {
			Door_RequestScheduledUnlock();
		}
		adminOverrideActive = true;
		return DOOR_ADMIN_OVERRIDE_OPENED;
	}

	if (!Door_CanCloseAdministrativeOverride()) {
		return DOOR_ADMIN_OVERRIDE_BUSY;
	}

	Door_RequestClose();
	adminOverrideActive = false;
	return DOOR_ADMIN_OVERRIDE_CLOSED;
}
