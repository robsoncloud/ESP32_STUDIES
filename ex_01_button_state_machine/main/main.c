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
        /* Get the current time */
        uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        /* Read the current state of the button */
        int current_button_state = gpio_get_level(BTN);
        uint32_t next = 0;
        /*
         * Identify a single press if:
         * - A button press was previously checked
         * - The time since the button was pressed is greater than 350 milliseconds
         */
        if (check_button_state == true && current_time - press_time > 350)
        {
            check_button_state = false;
            current_state = SINGLE_PRESS;
            press_count = 0;
            printf("Single Press identified\n");
        }
        
        // simple debounce
        if (current_time > next)
        {

            /* Check if the button state has changed from released to pressed */
            if (current_button_state == 0 && last_button_state == 1)
            {
                /* Record the time the button was pressed */
                press_time = current_time;
                next = current_time + 30;
            }
            /* Check if the button state has changed from pressed to released */
            else if (current_button_state == 1 && last_button_state == 0)
            {
                /* Calculate the time since the button was pressed */
                total_press_time = current_time - press_time;

                /* Increment the press count */
                press_count++;

                /* Check if the press was a long press */
                if (total_press_time > 1000)
                {
                    /* Reset state variables */
                    check_button_state = false;
                    press_count = 0;
                    printf("Long Press identified\n");
                }
                else if (total_press_time < 300 && press_count == 2)
                {
                    /* Reset state variables */
                    check_button_state = false;
                    current_state = DOUBLE_PRESS;
                    press_count = 0;
                    printf("Double Press identified\n");
                }
                else
                {
                    /* Wait for the button to be released and pressed again */
                    check_button_state = true;
                }

                /* Print debug information */
                printf("Total press time : %lu\n", (unsigned long)total_press_time);
                printf("Count: %d\n", press_count);
                printf("----------------------------------------------------\n");
                printf(" ");
            }
            else if (current_button_state == 0 && last_button_state == 0)
            {
                total_press_time = current_time - press_time;
                if (total_press_time > 1000 && current_state != LONG_PRESS)
                {
                    current_state = LONG_PRESS;
                    printf("Long Press identified - took: %lu\n", (unsigned long)total_press_time);
                }
            }
        }

        // Delay for 10 milliseconds to debounce the button press
        vTaskDelay(10 / portTICK_PERIOD_MS);

        // Update the last button state
        last_button_state = current_button_state;

        // Check the current state of the button press
        switch (current_state)
        {
        // If a single press, turn on the LED
        case SINGLE_PRESS:
            led_state = true;
            break;
        // If a double press, turn off the LED
        case DOUBLE_PRESS:
            led_state = false;
            break;
        // If a long press, toggle the LED and delay for 100 milliseconds
        case LONG_PRESS:
            led_state = !led_state;
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;
        // If no state is recognized, do nothing
        default:
            break;
        }

        // Set the LED state based on the current state
        gpio_set_level(LED, led_state);
    }

    vTaskSuspend(NULL);
}