#include "door_control.h"

#include "door.h"
#include "signalling.h"

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

__weak bool Door_ToggleAdministrativeOverride(void)
{
	if (Door_IsOpen()) {
		Door_RequestClose();
		return false;
	}

	Indicator_Signal_Authorised();
	Door_RequestScheduledUnlock();
	return true;
}
