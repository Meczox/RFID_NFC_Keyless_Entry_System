#include "pn532.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

#define PN532_ADDR   0x48   // the chip's I2C address

/*
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
 * just try to read back from the carrd but len + 1 cus of status bit at the front
 *
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

    uint8_t samConfig[] = { 0x14, 0x01, 0x14, 0x00 };

    if (!PN532_SendCommand(samConfig, sizeof(samConfig))) {
        return false;
    }

    // Clear out the ACK reply
    PN532_WaitAndRead(ack, 6, 100);

    HAL_Delay(50);
    return true;
}

bool PN532_ScanUID(uint8_t *uid, uint8_t *len)
{
    uint8_t ack[6];
    uint8_t resp[24];

    // command to make it scan for card
    uint8_t scan[] = { 0x4A, 0x01, 0x00 };

    PN532_SendCommand(scan, sizeof(scan));

    if (!PN532_WaitAndRead(ack, 6, 100))   return false;
    if (!PN532_WaitAndRead(resp, 24, 100)) return false;


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
