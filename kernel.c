#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/structs/systick.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "pico/stdlib.h"
#include "task.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint LED_PIN_T1 = 10;
const uint LED_PIN_T2 = 11;
const uint LED_PIN_T3 = 12;

int task1_val, task2_val, task3_val;
static int turn = 0;
static uint32_t task1_stack[128];
void task1();
static uint32_t task2_stack[128];
void task2();
static uint32_t task3_stack[128];
void task3();
void end_task(void);

// Fonctions définies en assembleur
extern void setup_systick(void);
extern void start_scheduler(void);
extern void isr_pendsv(void);
extern void isr_systick(void);

int main() {
    sleep_ms(2000);
    gpio_init(LED_PIN);
    gpio_init(LED_PIN_T1);
    gpio_init(LED_PIN_T2);
    gpio_init(LED_PIN_T3);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(LED_PIN_T1, GPIO_OUT);
    gpio_set_dir(LED_PIN_T2, GPIO_OUT);
    gpio_set_dir(LED_PIN_T3, GPIO_OUT);
    stdio_init_all();
    setup_systick();

    add_task(1, task1, task1_stack, 128);
    add_task(2, task2, task2_stack, 128);
    add_task(3, task3, task3_stack, 128);
    
    current_task = &tasks[0];
    start_scheduler();

    while (true);
    return 0;
}

// Task 1
void task1(void) {
    printf("[ TASK 1 ]\n");
    while (1) {
        gpio_put(LED_PIN_T1, 1);
        task1_val += 1;
        if (task1_val % 100000 == 0) 
            printf("Task 1\n");
    }
}

// Task 2
void task2(void) {
    printf("[ TASK 2 ]\n");
    while(true) {
        gpio_put(LED_PIN_T2, 1);
        task2_val += 1;
        if (task2_val % 100000 == 0) printf("Task 2\n");
    }
}

int i = 0;
// Task 3
void task3(void) {
    while (i < 1000000) {
        gpio_put(LED_PIN_T3, 1);
        if (i % 100000 == 0) printf("Task 3\n");
        i++;
    }
    gpio_put(LED_PIN_T3, 0);
    return;
}

void link_gpio_put(int pin, int value) {
    gpio_put(pin, value);
}

void context_switch_led_on() {
    gpio_put(LED_PIN, 1);
}

void context_switch_led_off() {
    gpio_put(LED_PIN, 0);
}