#ifndef INC_DOOR_CONTROL_H_
#define INC_DOOR_CONTROL_H_

#include <stdbool.h>

typedef enum {
    DOOR_ADMIN_OVERRIDE_OPENED = 0,
	DOOR_ADMIN_OVERRIDE_CLOSED,
	DOOR_ADMIN_OVERRIDE_BUSY
} DoorAdminOverrideResult_t;

/*
 * Integration points for the door/motor module. Strong implementations with
 * the same names can replace the weak placeholders in door_control.c.
 */
void Door_RequestAuthorizedEntry(void);
void Door_RequestScheduledUnlock(void);
void Door_RequestClose(void);
void Door_ReportUnauthorizedCredential(void);

DoorAdminOverrideResult_t Door_ToggleAdministrativeOverride(void);
bool Door_IsAdministrativeOverrideActive(void);
bool Door_CanCloseAdministrativeOverride(void);

#endif /* INC_DOOR_CONTROL_H_ */
