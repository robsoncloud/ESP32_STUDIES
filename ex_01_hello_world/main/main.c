#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

#define BTN 33
#define LED 23

#define ESP_INTR_FLAG_DEFAULT 0
static void IRAM_ATTR ISR_Handler(void *args);


static void init(void) {
    gpio_config_t io_led = {};
    io_led.mode = GPIO_MODE_OUTPUT;
    io_led.pin_bit_mask = (1ULL << LED);
    io_led.pull_down_en = 0;
    io_led.pull_up_en = 0;
    io_led.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_led);

    gpio_config_t io_btn = {};
    io_btn.mode = GPIO_MODE_INPUT;
    io_btn.pin_bit_mask = (1ULL << BTN);
    io_btn.pull_up_en = 0;
    io_btn.pull_down_en = 0;
    io_btn.intr_type = GPIO_INTR_ANYEDGE;

    // install ISR and
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BTN, ISR_Handler, (void *) BTN);

    gpio_config(&io_btn);

}

bool turn = false;

static void IRAM_ATTR ISR_Handler(void *args) {
    ets_printf("button released\n");
}

void app_main(void) {
    init();
    while(1) {
        gpio_set_level(LED, 1);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        gpio_set_level(LED, 0);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

}