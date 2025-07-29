#include "button/button.h"

volatile bool button_on_pressed = false;
volatile uint64_t last_button_on_press_time = 0;
event_motor_command_type_t button_start_last_event = MOTOR_CMD_STOP;

void IRAM_ATTR button_on_isr_handler(void* arg) {
    uint64_t current_time = esp_timer_get_time();  
    
    if (current_time - last_button_on_press_time > 50000) {
        button_on_pressed = true;
        last_button_on_press_time = current_time;
    }
}

void button_start_motor_handler()
{
    while (1) {
        if (button_on_pressed) {
            printf("Buton on pressed !\n");

            button_start_last_event = button_start_last_event == MOTOR_CMD_START ? MOTOR_CMD_STOP : MOTOR_CMD_START;

            event_t button_start_event = {
                .type = EVENT_MOTORS_COMMAND,
                .data = {
                    .motors_command = {
                        .type = button_start_last_event
                    }
                }
            };

            event_bus_publish(&button_start_event);

            ESP_LOGI(BUTTON_START_TAG, "Notification of pressed is send");

            button_on_pressed = false;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void button_start_motor_init() 
{
    // CONFIG ON BUTTON
    gpio_set_direction(BUTTON_ON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_ON_PIN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_ON_PIN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_ON_PIN, button_on_isr_handler, NULL);

    xTaskCreate(
        button_start_motor_handler,
        "btn_motor_start",
        4096,
        NULL,
        5,
        NULL
    );
}

void button_init() {
    printf("INIT BUTTONS \n");

    button_start_motor_init();

    printf("INIT BUTTONS DONE \n");
}