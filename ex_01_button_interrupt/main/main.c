#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
// debounce in ISR
#include "esp_timer.h"

// import queue
#include "freertos/queue.h"

#define BTN 33
#define LED 23
#define BTN_PIN_SEL (1ULL << BTN)
#define LED_PIN_SEL (1ULL << LED)
#define ESP_INTR_FLAG_DEFAULT 0
#define DEBOUNCE_TIME 50
static void IRAM_ATTR ISR_Handler(void *args);
static volatile uint interrupt_counter = 0;
static uint64_t last_press_time = 0;

// create static queue obj
static QueueHandle_t gpio_evt_queue = NULL;

static void configure_btn(void)
{
    gpio_config_t io = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BTN_PIN_SEL,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE};

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BTN, ISR_Handler, (void *)BTN);

    gpio_config(&io);
}

static void configure_led(void)
{
    gpio_config_t io = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = LED_PIN_SEL,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&io);
}

static void IRAM_ATTR ISR_Handler(void *args)
{
    uint64_t current_time = esp_timer_get_time() / 1000;
    uint32_t gpio_num = (uint32_t)args;
    uint32_t passed_time = current_time - last_press_time;
    if (passed_time > DEBOUNCE_TIME)
    {
        xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
        ets_printf("Called %d - Current time %lu - Btn %d - Time Difference - %lu\n", interrupt_counter, (unsigned long)current_time, gpio_num, passed_time);
        last_press_time = current_time;
    }
}

// static bool debounce(int pin)
// {
//     int initial_state = gpio_get_level(pin);
//     vTaskDelay(DEBOUNCE_TIME / portTICK_PERIOD_MS);
//     int final_state = gpio_get_level(pin);
//     return initial_state == final_state;
// }

// create task
static void gpio_task(void *pvParameters)
{
    uint32_t io_num;
    // printf("queue created\n");
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            interrupt_counter++;
            gpio_set_level(LED, interrupt_counter % 2);
            printf("GPIO[%lu] intr, count: %d\n", io_num, interrupt_counter);
        }
    }
}

void app_main(void)
{
    configure_btn();
    configure_led();
    
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    // while (1)
    // {
    //     // if (debounce(BTN))
    //     // {

    //     vTaskDelay(100 / portTICK_PERIOD_MS);
    //     // }
    // }

    vTaskSuspend(NULL);
}
