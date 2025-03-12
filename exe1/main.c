#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int timer_flag = 0;

bool timer_callback(repeating_timer_t *rt) {
    timer_flag = 1;
    return true; // keep repeating
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = 1;
    } else if(events == 0x8){}
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);                      

    repeating_timer_t timer;
    int timer_T = 500000;
    int led_status = 0;
    int timer_ligado = 0;

    while (true) {

        if(timer_flag){
            led_status = 1 - led_status;            
            gpio_put(LED_PIN_R, led_status);
            timer_flag = 0;
        }

        if (flag_f_r) {
            flag_f_r = 0;
            
            if (!timer_ligado){
                if (add_repeating_timer_us(timer_T, timer_callback, NULL, &timer)){
                    timer_ligado = 1;
                } 
            } else {
                cancel_repeating_timer(&timer);
                led_status = 0;
                timer_ligado = 0;
                gpio_put(LED_PIN_R, led_status);
            }
        }

        printf("%d", led_status);
    }
}
