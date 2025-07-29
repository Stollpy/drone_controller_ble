#ifndef EVENT_BLE_H
#define EVENT_BLE_H

typedef enum {
    BLE_STATE_DISCONNECTED = 0,
    BLE_STATE_CONNECTED = 1,
    BLE_STATE_CONNECTING = 2,
    BLE_STATE_ERROR = 3
} event_ble_state_type_t;

typedef struct {
    event_ble_state_type_t state;
} event_ble_state;

#endif 