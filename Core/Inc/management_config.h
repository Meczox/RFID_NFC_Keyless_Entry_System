#ifndef INC_MANAGEMENT_CONFIG_H_
#define INC_MANAGEMENT_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t unlockHour;
	uint8_t unlockMinute;
	uint8_t specificDateOpenHour;
	uint8_t specificDateOpenMinute;
	uint8_t unlockDay;
	uint8_t unlockMonth;
	uint16_t unlockYear;
	uint16_t unlockDurationSeconds;
	uint16_t specificDateDurationSeconds;
	bool useSpecificDate;
	bool scheduleEnabled;
} ManagementConfig_t;

void ManagementConfig_Init(void);
const ManagementConfig_t *ManagementConfig_Get(void);

bool ManagementConfig_SetUnlockTime(uint8_t hour, uint8_t minute);
bool ManagementConfig_SetUnlockDate(uint8_t day, uint8_t month, uint16_t year);
bool ManagementConfig_SetUnlockDuration(uint16_t durationSeconds);
bool ManagementConfig_SetSpecificDateOpenTime(uint8_t hour, uint8_t minute);
bool ManagementConfig_SetSpecificDateDuration(uint16_t durationSeconds);
void ManagementConfig_ClearUnlockDate(void);
void ManagementConfig_SetScheduleEnabled(bool enabled);

void ManagementConfig_EnterAdminMode(void);
void ManagementConfig_ExitAdminMode(void);
bool ManagementConfig_IsAdminMode(void);

#endif /* INC_MANAGEMENT_CONFIG_H_ */
