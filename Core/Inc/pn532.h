#ifndef PN532_SIMPLE_H
#define PN532_SIMPLE_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

/* Call once at startup, after MX_I2C1_Init(). Returns true if the chip
 * responded correctly. */
bool PN532_Begin(void);

/* Call in your loop. Returns true and fills uid/len if a card is present
 * right now. Returns false quickly if no card is there - safe to call
 * every loop iteration. */
bool PN532_ScanUID(uint8_t *uid, uint8_t *len);

// Id Tag Has UID
static const uint8_t TAG_UID[] = { 0x81, 0xE5, 0x1C, 0x07 };

static const uint8_t CARD_UID[] = {0xBB, 0x74, 0x0B, 0x07};
#endif
