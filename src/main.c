#include "config.h"
#include "ble/ble.h"
#include "button/button.h"


//MAC: d0:ef:76:c3:cd:ec
void app_main() {
    printf("Start ... \n");

    button_init();
    ble_init();

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}