#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

#define GPIO_LED 2
#define GPIO_BUTTON 5
#define GPIO_LED_PIN_SEL (1ULL << GPIO_LED)
#define GPIO_BUTTON_PIN_SEL (1ULL << GPIO_BUTTON)

#define ESP_INTR_FLAG_DEFAULT 0

static void IRAM_ATTR ISR_Handler(void *args);

static TickType_t next = 0;
static bool led_status = false;
TaskHandle_t TaskHandler;

static void TaskFunction(void *pvParameters)
{
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ets_printf("button pressed");
    }
}

static void IRAM_ATTR ISR_Handler(void *args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /*
        BaseType_t is a FreeRTOS data type used for variables that hold a simple status value.
        xHigherPriorityTaskWoken is used to record whether a context switch is required following the ISR.
        It's initialized to pdFALSE, meaning that, initially, we assume no higher priority task has been woken by this ISR.
    */

    vTaskNotifyGiveFromISR(TaskHandler, &xHigherPriorityTaskWoken);
    /*
        vTaskNotifyGiveFromISR is a FreeRTOS function used to notify a task from an ISR.
        TaskHandle is a handle to the task that should be notified. This handle is usually obtained when the task is created using xTaskCreate.
        &xHigherPriorityTaskWoken is a pointer to xHigherPriorityTaskWoken. The function will set this to pdTRUE if notifying the task results in a task of higher priority than the currently running task being woken.
    */

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    /*
        This line is crucial for ensuring real-time responsiveness in FreeRTOS.
        portYIELD_FROM_ISR is a macro/function used to trigger a context switch from an ISR.
        If xHigherPriorityTaskWoken was set to pdTRUE by vTaskNotifyGiveFromISR, this means a higher priority task is now ready to run and should be switched to immediately.
        portYIELD_FROM_ISR checks the value of xHigherPriorityTaskWoken, and if it's pdTRUE, it triggers a context switch to the higher priority task.
    */
}
static void init_hw(void)
{
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_LED_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_BUTTON_PIN_SEL;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_BUTTON, ISR_Handler, NULL);
}

void app_main(void)
{
    init_hw();
    xTaskCreate(TaskFunction, "handleLed", 1024, NULL, configMAX_PRIORITIES - 1, &TaskHandler);
    vTaskSuspend(NULL);
}
