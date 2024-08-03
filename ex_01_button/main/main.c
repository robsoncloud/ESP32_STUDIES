#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED 23
#define BTN 33

typedef enum
{
     LED_OFF,
     LED_ON,
     LED_BLINK
} led_mode_t;

static void configure_btn(void)
{
     gpio_config_t io = {};
     io.mode = GPIO_MODE_INPUT;
     io.intr_type = GPIO_INTR_DISABLE;
     io.pin_bit_mask = (1ULL << BTN);
     io.pull_up_en = GPIO_PULLUP_ENABLE;
     gpio_config(&io);
}

static void configure_led(void)
{
     gpio_config_t io = {};
     io.mode = GPIO_MODE_OUTPUT;
     io.intr_type = GPIO_INTR_DISABLE;
     io.pin_bit_mask = (1ULL << LED);
     gpio_config(&io);
}

void app_main(void){
     configure_btn();
     configure_led();
     
     bool led_state = false;
     led_mode_t led_mode = LED_OFF;
     int last_button_state = 1;

     while(1) {
          int current_button_state = gpio_get_level(BTN);

          if(current_button_state == 0 && last_button_state == 1) {
               led_mode = (led_mode + 1) % 3;
               printf("LED MODE: %s\n", led_mode == 0 ? "OFF" : "ON" );
               vTaskDelay(50 / portTICK_PERIOD_MS);
          }

          switch(led_mode) {
               case LED_OFF:
                    led_state = false;
                    break;
               case LED_ON:
                    led_state = true;
                    break;
               case LED_BLINK:
                    led_state = !led_state;
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    break;
          }

          last_button_state = current_button_state;
          gpio_set_level(LED, led_state);
          vTaskDelay(100 / portTICK_PERIOD_MS);

     }
}
