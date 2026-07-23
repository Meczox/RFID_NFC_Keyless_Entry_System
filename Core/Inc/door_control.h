#ifndef INC_DOOR_CONTROL_H_
#define INC_DOOR_CONTROL_H_

#include <stdbool.h>

/*
 * Integration points for the door/motor module. Strong implementations with
 * the same names can replace the weak placeholders in door_control.c.
 */
void Door_RequestAuthorizedEntry(void);
void Door_RequestScheduledUnlock(void);
void Door_RequestClose(void);
void Door_ReportUnauthorizedCredential(void);
bool Door_ToggleAdministrativeOverride(void);

#endif /* INC_DOOR_CONTROL_H_ */
