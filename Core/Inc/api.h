typedef enum {
    AUTH_DENIED = 0,
    AUTH_USER_ENTRY,
    AUTH_ADMIN_OVERRIDE
} AuthStatus_t;

typedef enum {
    SENSOR_CLEAR = 0,
    SENSOR_BLOCKED
} SensorState_t;

typedef enum {
    STATE_IDLE,
    STATE_AUTHORIZING,
    STATE_DOOR_OPEN_ENTRY,
    STATE_DOOR_OPEN_EXIT,
    STATE_ALARM,
    STATE_ADMIN_CONFIG,
    STATE_SPECIAL_EVENT_OPEN
} SystemState_t;

bool NFC_Read_UID(uint8_t* uid_buffer);

AuthStatus_t Access_Verify_UID(const uint8_t* uid);

void Sensors_Init(void);

SensorState_t Sensor_Get_Entry_LDR(void);

SensorState_t Sensor_Get_Exit_LDR(void);

bool Sensor_Is_Doorway_Occupied(void);

void Indicator_Signal_Authorized(void);

void Alarm_Trigger_Intrusion(void);

void Alarm_Clear(void);

char Keypad_Get_Key(void);

void UI_Display_Status(const char* status_msg);

void UI_Run_Admin_Menu(void);

SystemState_t FSM_Determine_Next_State(SystemState_t current_state);
