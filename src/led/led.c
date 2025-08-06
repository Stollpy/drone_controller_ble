#include "led/led.h"


void led_ble_connected() {
    gpio_set_level(LED_BLE_CONNECTED_PIN, 1);
}

void led_ble_disconnected() {
    gpio_set_level(LED_BLE_CONNECTED_PIN, 0);
}

void led_init() {
    gpio_set_direction(LED_BLE_CONNECTED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_BLE_CONNECTED_PIN, 0);

    event_bus_subscribe(EVENT_BLE_CONNECTED, led_ble_connected);
    event_bus_subscribe(EVENT_BLE_DISCONNECTED, led_ble_disconnected);
}