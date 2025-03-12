#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

const repeating_timer_t timer_r;
const repeating_timer_t timer_g;

volatile int flag_r = 0;
volatile int flag_g = 0;
volatile int timer_r_flag = 0;
volatile int timer_g_flag = 0;

bool timer_callback(repeating_timer_t *rt) {
    if (rt == &timer_g){
        timer_g_flag = 1;
    } else if (rt == &timer_r){
        timer_r_flag = 1;
    }
    return true; // keep repeating
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int timer_T_r = 500000;
    int led_r_status = 0;
    int timer_r_ligado = 0;

    int timer_T_g = 250000;
    int led_g_status = 0;
    int timer_g_ligado = 0;

    while (true) {

        if(timer_g_flag){
            led_g_status = 1 - led_g_status;            
            gpio_put(LED_PIN_G, led_g_status);
            timer_g_flag = 0;
        }

        if(timer_r_flag){
            led_r_status = 1 - led_r_status;            
            gpio_put(LED_PIN_R, led_r_status);
            timer_r_flag = 0;
        }

        if (flag_r) {
            flag_r = 0;

            if (!timer_r_ligado){
                if (add_repeating_timer_us(timer_T_r, timer_callback, NULL, &timer_r)){
                    timer_r_ligado = 1;
                } 
            } else {
                cancel_repeating_timer(&timer_r);
                led_r_status = 0;
                timer_r_ligado = 0;
                gpio_put(LED_PIN_R, led_r_status);
            }
        }

        if (flag_g) {
            flag_g = 0;

            if (!timer_g_ligado){
                if (add_repeating_timer_us(timer_T_g, timer_callback, NULL, &timer_g)){
                    timer_g_ligado = 1;
                } 
            } else {
                cancel_repeating_timer(&timer_g);
                led_g_status = 0;
                timer_g_ligado = 0;
                gpio_put(LED_PIN_G, led_g_status);
            }
        }
    }
}
