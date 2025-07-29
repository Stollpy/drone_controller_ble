// events.h
#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include <string.h>

#include "event/event_motor.h"
#include "event/event_ble.h"

typedef enum {
    EVENT_BLE_CONNECTED,
    EVENT_BLE_DISCONNECTED,
    EVENT_MOTORS_UPDATE,
    EVENT_MOTORS_COMMAND,
    EVENT_JOYSTICK_DIRECTION,
    EVENT_UNKNOWN
} event_type_t;

typedef struct {
    event_type_t type;
    union {
        event_motors_command_data_t motors_command;
        event_ble_state ble_state;
    } data;
} event_t;

typedef void (*event_handler_t)(event_t *event);

void event_bus_subscribe(event_type_t type, event_handler_t handler);
void event_bus_publish(event_t *event);

#endif
