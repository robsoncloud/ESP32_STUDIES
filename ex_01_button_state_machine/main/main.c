#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BTN 33
#define LED 23

typedef enum
{
    IDLE,
    SINGLE_PRESS,
    DOUBLE_PRESS,
    LONG_PRESS

} button_state_t;

static void configure_btn(void)
{
    gpio_config_t io = {};
    io.mode = GPIO_MODE_INPUT;
    io.pin_bit_mask = (1ULL << BTN);
    io.intr_type = GPIO_INTR_DISABLE;
    io.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io);
}

static void configure_led(void)
{
    gpio_config_t io = {};
    io.mode = GPIO_MODE_OUTPUT;
    io.pin_bit_mask = (1ULL << LED);
    gpio_config(&io);
}

static button_state_t current_state = IDLE;
static int last_button_state = 1;
static uint32_t press_time = 0;
static uint32_t total_press_time = 0;
static int press_count = 0;
void app_main(void)
{
    configure_btn();
    configure_led();
    bool led_state = false;
    bool check_button_state = false;
    while (1)
    {
        uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        int current_button_state = gpio_get_level(BTN);

        if (check_button_state == true && current_time - press_time > 350)
        {
            check_button_state = false;
            current_state = SINGLE_PRESS;
            press_count = 0;
            printf("Single Press identified\n");
        }

        if (current_button_state == 0 && last_button_state == 1)
        {
            press_time = current_time;
        }
        else if (current_button_state == 1 && last_button_state == 0)
        {

            total_press_time = current_time - press_time;

            press_count++;

            if (total_press_time > 1000)
            {
                check_button_state = false;
                current_state = LONG_PRESS;
                press_count = 0;
                printf("Long Press identified\n");
            }
            else if (total_press_time < 300 && press_count == 2)
            {
                check_button_state = false;
                current_state = DOUBLE_PRESS;
                press_count = 0;
                printf("Double Press identified\n");
            }
            else
            {
                check_button_state = true;
            }

            printf("Total press time : %lu\n", (unsigned long)total_press_time);
            printf("Count: %d\n", press_count);
            printf("----------------------------------------------------\n");
            printf(" ");
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
        last_button_state = current_button_state;

        switch (current_state)
        {
        case SINGLE_PRESS:
            led_state = true;
            break;
        case DOUBLE_PRESS:
            led_state = false;
            break;
        case LONG_PRESS:
            led_state = !led_state;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;
        default:
            break;
        }

        gpio_set_level(LED, led_state);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
}