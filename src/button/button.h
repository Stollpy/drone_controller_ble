#ifndef BUTTON
#define BUTTON

#include "driver/gpio.h"
#include "esp_timer.h"

#include "config.h"
#include "event/event.h"

#define BUTTON_START_TAG "BUTTON_START"
#define BUTTON_ON_PIN GPIO_NUM_18

void button_init();
#endif