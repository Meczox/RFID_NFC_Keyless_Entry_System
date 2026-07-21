#include "pn532.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

#define PN532_ADDR   0x48   // the chip's I2C address

/*
 * PN532 must be wrapped in this exact byte
 *
 *   00 00 FF  LEN  LCS  D4  < command bytes>  DCS  00
 *
 *   LEN = number of bytes from D4 to the end of your command
 *   LCS = a checksum of LEN (so LEN + LCS always adds to 0)
 *   D4  = fixed byte meaning "this message is from host to chip"
 *   DCS = a checksum of D4 + your command bytes (so they all add to 0)
 */
static bool PN532_SendCommand(uint8_t *cmd, uint8_t cmdLen)
{
    uint8_t frame[32];
    uint8_t i = 0;
    uint8_t checksum = 0xD4;

    frame[i++] = 0x00;
    frame[i++] = 0x00;
    frame[i++] = 0xFF;
    frame[i++] = cmdLen + 1;
    frame[i++] = (uint8_t)(~(cmdLen + 1) + 1);
    frame[i++] = 0xD4;

    for (uint8_t j = 0; j < cmdLen; j++) {
        frame[i++] = cmd[j];
        checksum = (uint8_t)(checksum + cmd[j]);
    }
    frame[i++] = (uint8_t)(~checksum + 1);
    frame[i++] = 0x00;

    return HAL_I2C_Master_Transmit(&hi2c1, PN532_ADDR, frame, i, 100) == HAL_OK;
}

/*
 * Every time we read from the PN532 over I2C, the very first byte back is
 * always a "ready?" flag (0x01 = yes, data follows / 0x00 = not yet) - this
 * happens on every single read, so we keep retrying until we see 0x01 or
 * time out.
 */
static bool PN532_WaitAndRead(uint8_t *out, uint8_t len, uint32_t timeoutMs)
{
    uint8_t raw[40];
    uint32_t start = HAL_GetTick();

    while ((HAL_GetTick() - start) < timeoutMs) {
        if (HAL_I2C_Master_Receive(&hi2c1, PN532_ADDR, raw, len + 1, 100) == HAL_OK) {
            if (raw[0] == 0x01) {
                memcpy(out, &raw[1], len);
                return true;
            }
        }
        HAL_Delay(1);
    }
    return false;
}

bool PN532_Begin(void)
{
    uint8_t ack[6];
    uint8_t resp[10];

    /* Command 0x02 = "GetFirmwareVersion" - just checks the chip is alive and talking. */
    uint8_t getVersion[] = { 0x02 };
    PN532_SendCommand(getVersion, sizeof(getVersion));
    if (!PN532_WaitAndRead(ack, 6, 100))  return false; /* the chip's "got it" reply */
    if (!PN532_WaitAndRead(resp, 10, 100)) return false; /* the actual version data */

    /* Command 0x14 = "SAMConfiguration" - must be sent once to put the chip
     * into normal card-scanning mode. */
    uint8_t samConfig[] = { 0x14, 0x01, 0x14, 0x01 };
    PN532_SendCommand(samConfig, sizeof(samConfig));
    PN532_WaitAndRead(ack, 6, 100);
    PN532_WaitAndRead(resp, 3, 100);

    return true;
}

bool PN532_ScanUID(uint8_t *uid, uint8_t *len)
{
    uint8_t ack[6];
    uint8_t resp[24];

    uint8_t scan[] = { 0x4A, 0x01, 0x00 };
    PN532_SendCommand(scan, sizeof(scan));

    if (!PN532_WaitAndRead(ack, 6, 100))   return false;
    if (!PN532_WaitAndRead(resp, 24, 200)) return false;


    if (resp[7] == 0) {
        return false; /* no card found this time */
    }

    uint8_t idLen = resp[12];
    if (idLen > 10) {
        idLen = 10;
    }
    memcpy(uid, &resp[13], idLen);
    *len = idLen;

    return true;

}
