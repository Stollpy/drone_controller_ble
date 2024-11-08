#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <string.h>

#define BUTTON_OFF_PIN GPIO_NUM_17
#define BUTTON_ON_PIN GPIO_NUM_16

#define TAG "BLE_CLIENT"

#define SERVICE_UUID 0x00FF
#define CHARACTERISTIC_UUID 0xFF01

volatile bool button_down_pressed = false;
volatile uint64_t last_button_down_press_time = 0;

volatile bool button_on_pressed = false;
volatile uint64_t last_button_on_press_time = 0;

static esp_gattc_char_elem_t *char_elem_result = NULL;

static esp_gatt_if_t gattc_if_global = 0;
static const uint8_t gatt_server_address[] = {0xd0, 0xef, 0x76, 0x1e, 0x5d, 0xee};
static uint16_t gatt_conn_id = 0; 

void IRAM_ATTR button_off_isr_handler(void* arg) {
        uint64_t current_time = esp_timer_get_time();  
    if (current_time - last_button_down_press_time > 50000) {
        button_down_pressed = true;
        last_button_down_press_time = current_time;
    }
}

void IRAM_ATTR button_on_isr_handler(void* arg) {
        uint64_t current_time = esp_timer_get_time();  
    if (current_time - last_button_on_press_time > 50000) {
        button_on_pressed = true;
        last_button_on_press_time = current_time;
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
                if (memcmp(param->scan_rst.bda, gatt_server_address, 6) == 0) {
                    ESP_LOGI(TAG, "Serveur GATT trouvé, connexion en cours...");
                    esp_ble_gap_stop_scanning();
                    esp_ble_gattc_open(gattc_if_global, param->scan_rst.bda, BLE_ADDR_TYPE_PUBLIC, true);
                }
            }
            break;
        default:
            break;
    }
}

void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
    switch (event) {
        case ESP_GATTC_REG_EVT:
            printf("Client GATT saved !");
            ESP_LOGI(TAG, "Client GATT saved !");
            gattc_if_global = gattc_if;
            esp_ble_gap_start_scanning(10);
            break;
        case ESP_GATTC_OPEN_EVT:
            printf("Connexion établie avec le serveur GATT");
            ESP_LOGI(TAG, "Connexion établie avec le serveur GATT");
            gatt_conn_id = param->open.conn_id;  // Sauvegarder conn_id lorsque la connexion est ouverte
            esp_ble_gattc_search_service(gattc_if, gatt_conn_id, NULL);
            break;
        case ESP_GATTC_SEARCH_CMPL_EVT:
            ESP_LOGI(TAG, "Search is finish");
            printf("Search is finish \n");
            break;
        default:
            break;
    }
}

void init_controller() {
    printf("INIT CONTROLLER START \n");
    // CONFIG OFF BUTTON
    gpio_set_direction(BUTTON_OFF_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_OFF_PIN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_OFF_PIN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_OFF_PIN, button_off_isr_handler, NULL);

    // CONFIG ON BUTTON
    gpio_set_direction(BUTTON_ON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_ON_PIN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_ON_PIN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_ON_PIN, button_on_isr_handler, NULL);

    printf("INIT CONTROLLER DONE \n");
}

void init_ble() {
    printf("INIT BLE \n");

    // Initialiser NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Libérer la mémoire du mode Classic BT
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    // Configurer le contrôleur Bluetooth
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    // Initialiser le Bluedroid
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    // Enregistrer les callbacks GAP et GATT
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gattc_register_callback(gattc_event_handler));

    // Définir le nom de l’appareil et démarrer le scanning
    esp_ble_gap_set_device_name("stolly_drone_controller");
    ESP_ERROR_CHECK(esp_ble_gap_start_scanning(10));

    printf("INIT BLE DONE \n");
}

//MAC: d0:ef:76:c3:cd:ec
void app_main() {
    printf("Start ... \n");

    init_controller();
    init_ble();

    while (1) {
        if (button_down_pressed) {
            printf("Buton off pressed !\n");
            
            uint8_t value = 1;
            esp_ble_gattc_write_char(gattc_if_global, gatt_conn_id, char_elem_result->char_handle, sizeof(value), &value, ESP_GATT_WRITE_TYPE_RSP, ESP_GATT_AUTH_REQ_NONE);
            ESP_LOGI(TAG, "Notification of pressed is send");

            button_down_pressed = false;
        }

        if (button_on_pressed) {
            printf("Buton on pressed !\n");

            uint8_t value = 1;
            esp_ble_gattc_write_char(gattc_if_global, gatt_conn_id, char_elem_result->char_handle, sizeof(value), &value, ESP_GATT_WRITE_TYPE_RSP, ESP_GATT_AUTH_REQ_NONE);
             ESP_LOGI(TAG, "Notification of pressed is send");

            button_on_pressed = false;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}