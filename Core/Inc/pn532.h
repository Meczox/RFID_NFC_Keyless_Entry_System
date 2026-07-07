#ifndef PN532_SIMPLE_H
#define PN532_SIMPLE_H

#include "main.h"
#include <stdbool.h>

/* Call once at startup, after MX_I2C1_Init(). Returns true if the chip
 * responded correctly. */
bool PN532_Begin(void);

/* Call in your loop. Returns true and fills uid/len if a card is present
 * right now. Returns false quickly if no card is there - safe to call
 * every loop iteration. */
bool PN532_ScanUID(uint8_t *uid, uint8_t *len);

#endif
