#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

#define LED_GPIO GPIO_NUM_2
#define BUTTON_GPIO GPIO_NUM_14

#define ESP_INTR_FLAG_DEFAULT 0

static void IRAM_ATTR ISR_handler(void *args);

static TickType_t next = 0;
static TaskHandle_t task_1;

static void MyFunction(void)
{
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        ets_printf("task started");
    }
}

static void init_hw()
{
    gpio_config_t io;

    // Configure LED pin
    io.mode = GPIO_MODE_OUTPUT;
    io.pin_bit_mask = (1ULL << LED_GPIO);
    io.pull_up_en = GPIO_PULLUP_DISABLE;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io);

    // Configure button pin
    io.mode = GPIO_MODE_INPUT;
    io.pin_bit_mask = (1ULL << BUTTON_GPIO);
    io.pull_up_en = GPIO_PULLUP_ENABLE;
    io.intr_type = GPIO_INTR_ANYEDGE;
    gpio_config(&io);

    // Install ISR service and add handler for button
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BUTTON_GPIO, ISR_handler, (void *)BUTTON_GPIO);
}

static void IRAM_ATTR ISR_handler(void *args)
{
    TickType_t now = xTaskGetTickCountFromISR();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (now > next)
    {
        next = now + 50 / portTICK_PERIOD_MS;
        if (gpio_get_level(BUTTON_GPIO) == 0)
        {

            ets_printf("button pressed\n");
            vTaskNotifyGiveFromISR(task_1, xHigherPriorityTaskWoken);
                }
        else
        {
            ets_printf("button released\n");
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void app_main()
{
    init_hw();
    printf("Hello, Wokwi!\n");
    xTaskCreate(MyFunction, "myTask", 1024, NULL, configMAX_PRIORITIES - 1, &task_1);
}
