#ifndef INC_MANAGEMENT_CONFIG_H_
#define INC_MANAGEMENT_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t unlockHour;
	uint8_t unlockMinute;
	uint16_t unlockDurationSeconds;
	bool scheduleEnabled;
} ManagementConfig_t;

void ManagementConfig_Init(void);
const ManagementConfig_t *ManagementConfig_Get(void);

bool ManagementConfig_SetUnlockTime(uint8_t hour, uint8_t minute);
bool ManagementConfig_SetUnlockDuration(uint16_t durationSeconds);
void ManagementConfig_SetScheduleEnabled(bool enabled);

void ManagementConfig_EnterAdminMode(void);
void ManagementConfig_ExitAdminMode(void);
bool ManagementConfig_IsAdminMode(void);

#endif /* INC_MANAGEMENT_CONFIG_H_ */
