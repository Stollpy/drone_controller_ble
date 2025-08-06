#ifndef LED
#define LED

#include "driver/gpio.h"
#include "event/event.h"
#include "config.h"

#define LED_BLE_CONNECTED_PIN GPIO_NUM_19

void led_init();

#endif