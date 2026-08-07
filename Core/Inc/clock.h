#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} ClockTime_t;

typedef struct {
	uint8_t day;
	uint8_t month;
	uint16_t year;
} ClockDate_t;

typedef struct {
	ClockDate_t date;
	ClockTime_t time;
} ClockDateTime_t;

bool Clock_Init(void);
bool Clock_GetTime(ClockTime_t *time);
bool Clock_SetTime(uint8_t hour, uint8_t minute, uint8_t second);
bool Clock_GetDate(ClockDate_t *date);
bool Clock_SetDate(uint8_t day, uint8_t month, uint16_t year);
bool Clock_GetDateTime(ClockDateTime_t *dateTime);

#endif /* INC_CLOCK_H_ */
