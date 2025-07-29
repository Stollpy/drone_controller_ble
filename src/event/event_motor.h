#ifndef EVENT_MOTOR
#define EVENT_MOTOR

typedef enum {
    MOTOR_CMD_STOP = 0,
    MOTOR_CMD_START = 1,
    // MOTOR_CMD_REVERSE = 2
} event_motor_command_type_t;

typedef struct {
    event_motor_command_type_t type;
} event_motors_command_data_t;

#endif
