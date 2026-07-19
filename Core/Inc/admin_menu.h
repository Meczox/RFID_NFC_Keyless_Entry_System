#ifndef INC_ADMIN_MENU_H_
#define INC_ADMIN_MENU_H_

#include <stdint.h>

typedef enum {
	ADMIN_MENU_INACTIVE = 0,  // not in admin menu
	ADMIN_MENU_MAIN,          // display the main menu
	ADMIN_MENU_SET_TIME,      // set unlock time
	ADMIN_MENU_SET_DURATION,  // set unlock duration
	ADMIN_MENU_SHOW_CONFIG,   // display current settings
	ADMIN_MENU_FEEDBACK       // briefly display save/error feedback
} AdminMenuState_t;

void AdminMenu_Init(void);
void AdminMenu_Enter(void);
void AdminMenu_Update(uint8_t key);
AdminMenuState_t AdminMenu_GetState(void);

#endif /* INC_ADMIN_MENU_H_ */
