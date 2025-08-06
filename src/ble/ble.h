#ifndef BLE
#define BLE

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "config.h"
#include "event/event.h"

#define GATTC_TAG "BLE_CLIENT"

#define APP_NUM 1

#define APP_MOTOR_ID 0
#define APP_MOTOR_CHAR_VAL_LEN_MAX 0x01
#define MOTOR_SERVICE_UUID 0x00FF
#define MOTOR_CHARACTERISTIC_UUID 0xFF01
#define MOTOR_DESCR_UUID 0x3333
#define MOTOR_HANDLE 0x04
#define MOTOR_STATE_START 0x01
#define MOTOR_STATE_STOP 0x00

void ble_init();
bool ble_is_connected();

#endif