#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} ClockTime_t;

bool Clock_Init(void);
bool Clock_GetTime(ClockTime_t *time);
bool Clock_SetTime(uint8_t hour, uint8_t minute, uint8_t second);

#endif /* INC_CLOCK_H_ */
