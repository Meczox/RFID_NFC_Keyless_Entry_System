#ifndef INC_ACCESS_CONTROL_H_
#define INC_ACCESS_CONTROL_H_

#include <stdint.h>

typedef enum {
	ACCESS_ROLE_UNKNOWN = 0,
	ACCESS_ROLE_USER,
	ACCESS_ROLE_ADMIN
} AccessRole_t;

/* Identify a scanned credential from its UID. */
AccessRole_t AccessControl_IdentifyUID(const uint8_t *uid, uint8_t uidLength);

#endif /* INC_ACCESS_CONTROL_H_ */
