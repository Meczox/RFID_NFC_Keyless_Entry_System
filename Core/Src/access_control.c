#include "access_control.h"

#include <stddef.h>
#include <string.h>

static const uint8_t ADMIN_TAG_UID[] = {0x81, 0xE5, 0x1C, 0x07};
static const uint8_t USER_CARD_UID[] = {0xBB, 0x74, 0x0B, 0x07};

static int uid_matches(const uint8_t *uid, uint8_t uidLength,
		const uint8_t *expected, size_t expectedLength)
{
	return uid != NULL && uidLength == expectedLength
			&& memcmp(uid, expected, expectedLength) == 0;
}

// to match whether the card is user or admin
AccessRole_t AccessControl_IdentifyUID(const uint8_t *uid, uint8_t uidLength)
{	
	// admin card
	if (uid_matches(uid, uidLength, ADMIN_TAG_UID, sizeof(ADMIN_TAG_UID))) {
		return ACCESS_ROLE_ADMIN;
	}


	// user card
	if (uid_matches(uid, uidLength, USER_CARD_UID, sizeof(USER_CARD_UID))) {
		return ACCESS_ROLE_USER;
	}

	return ACCESS_ROLE_UNKNOWN;
}
