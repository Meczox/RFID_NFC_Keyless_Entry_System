#ifndef DOOR_H
#define DOOR_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    DOOR_CLOSED = 0,
    DOOR_OPEN,
    DOOR_MOVING_ENTRY,
    DOOR_MOVING_EXIT
} DoorState_t;

void Door_Init(void);
void Door_OpenForEntry(void);
void Door_OpenForExit(void);
void Door_Close(void);

DoorState_t Door_GetState(void);
bool Door_IsOpen(void);
bool Door_IsClosed(void);

#endif
