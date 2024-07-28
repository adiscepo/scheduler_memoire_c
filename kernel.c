#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/structs/systick.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "pico/stdlib.h"
#include "scheduler.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint LED_PIN_T1 = 10;
const uint LED_PIN_T2 = 11;
const uint LED_PIN_T3 = 12;

int task0_val, task1_val, task2_val;
static int turn = 0;
void task0();
void task1();
void task2();
void end_task(void);

extern scheduler_t scheduler;

int main() {
    gpio_init(LED_PIN);
    gpio_init(LED_PIN_T1);
    gpio_init(LED_PIN_T2);
    gpio_init(LED_PIN_T3);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(LED_PIN_T1, GPIO_OUT);
    gpio_set_dir(LED_PIN_T2, GPIO_OUT);
    gpio_set_dir(LED_PIN_T3, GPIO_OUT);
    stdio_init_all();
    init_scheduler();

    create_process(8000, task0);
    create_process(1500, task1);
    create_process(1000, task2);

    printf("Task: %d\n", scheduler.current_process);

    start_scheduler();
    
    return 0;
}

// Task 0
void task0(void) {
    // printf("[ Début tâche 1 %d %d\n", schedule.current_process, NOW);
    printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T1, 1);
    while (task0_val < 10000000) {
        task0_val += 1;
        if (task0_val % 100000 == 0) printf("Task 0\n");
    }
    // printf("[ Fin tâche 1 %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T1, 0);
    task0_val = 0;
    printf("F %d %d\n", scheduler.current_process, NOW);
}

// Task 1
void task1(void) {
    printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T2, 1);
    while(task1_val < 2000000) {
        task1_val += 1;
        // if (task1_val % 100000 == 0) printf("Task 1\n");
    }
    // printf("[ Fin tâche 2 %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T2, 0);
    task1_val = 0;
    printf("F %d %d\n", scheduler.current_process, NOW);
}

int i = 0;
// Task 2
void task2(void) {
    printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T3, 1);
    while (i < 1000000) {
        // if (i % 100000 == 0) printf("Task 2\n");
        i++;
    }
    gpio_put(LED_PIN_T3, 0);
    i = 0;
    printf("D %d %d\n", scheduler.current_process, NOW);
    return;
}