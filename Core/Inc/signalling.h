#ifndef SIGNALLING_H
#define SIGNALLING_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SIGNAL_IDLE = 0,
    SIGNAL_AUTHORISED,
    SIGNAL_ADMIN,
    SIGNAL_DOOR_OPEN,
    SIGNAL_UNAUTHORISED
} SignalPattern_t;

void Signalling_Init(void);
void Signalling_AllOff(void);

void Indicator_Signal_Authorised(void);
void Indicator_Signal_Admin(void);
void Indicator_Signal_DoorOpen(void);
void Indicator_Signal_DoorClosed(void);

void Alarm_Trigger_Unauthorised(void);
void Alarm_Clear(void);

void Signalling_RunTask(void);

#endif
