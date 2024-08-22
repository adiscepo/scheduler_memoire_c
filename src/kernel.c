#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/structs/systick.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "pico/stdlib.h"
#include "scheduler.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint LED_PIN_T0 = 10;
const uint LED_PIN_T1 = 11;
const uint LED_PIN_T2 = 12;

void task0();
void task1();
void task2();
void end_task(void);

extern scheduler_t scheduler;

int main()
{
    gpio_init(LED_PIN);
    gpio_init(LED_PIN_T0);
    gpio_init(LED_PIN_T1);
    gpio_init(LED_PIN_T2);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(LED_PIN_T0, GPIO_OUT);
    gpio_set_dir(LED_PIN_T1, GPIO_OUT);
    gpio_set_dir(LED_PIN_T2, GPIO_OUT);
    stdio_init_all();
    init_scheduler();

    create_process(2500, task0);
    create_process(1000, task1);
    create_process(500, task2);

    debug_printf("Task: %d\n", scheduler.current_process);

    start_scheduler();

    return 0;
}

// Task 0
void task0(void)
{
    // debug_printf("[ Début tâche 1 %d %d\n", schedule.current_process, NOW);
    debug_printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T0, 1);
    uint32_t task0_val = 0;
    while (task0_val < 10000000) // 10 000 000
    {
        task0_val += 1;
        if (task0_val % 100000 == 0) // 100 000
            debug_printf("Task 0\n");
    }
    // debug_printf("[ Fin tâche 1 %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T0, 0);
    task0_val = 0;
    debug_printf("F %d %d\n", scheduler.current_process, NOW);
}

// Task 1
void task1(void)
{
    debug_printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T1, 1);
    uint32_t task1_val = 0;
    while (task1_val < 2000000) // 2 000 000
    {
        task1_val += 1;
        if (task1_val % 100000 == 0) // 100 000 
            debug_printf("Task 1\n");
    }
    // debug_printf("[ Fin tâche 2 %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T1, 0);
    task1_val = 0;
    debug_printf("F %d %d\n", scheduler.current_process, NOW);
}

// Task 2
void task2(void)
{
    debug_printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T2, 1);
    uint32_t task2_val = 0;
    while (task2_val < 1000000) // 1 000 000
    {
        if (task2_val % 100000 == 0) // 100 000 
            debug_printf("Task 2\n");
        task2_val++;
    }
    gpio_put(LED_PIN_T2, 0);
    task2_val = 0;
    debug_printf("F %d %d\n", scheduler.current_process, NOW);
    return;
}