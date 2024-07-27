#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/structs/systick.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "pico/stdlib.h"
#include "task.h"
#include "scheduler.h"

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
    setup_systick();
    init_scheduler();

    __asm("CPSID I");
    create_process(7000, 7000, task1);
    create_process(1000, 1000, task2);
    create_process(1500, 1500, task3);
    __asm("CPSIE I");

    printf("Task: %d\n", scheduler.current_process);

    // current_task = &tasks[0];
    start_scheduler();
    
    // while (true);
    return 0;
}

// Task 1
void task1(void) {
    // printf("[ Début tâche 1 %d %d\n", schedule.current_process, NOW);
    printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T1, 1);
    while (task1_val < 10000000) {
        task1_val += 1;
        if (task1_val % 100000 == 0) printf("Task 1\n");
    }
    // printf("[ Fin tâche 1 %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T1, 0);
    task1_val = 0;
    printf("F %d %d\n", scheduler.current_process, NOW);
}

// Task 2
void task2(void) {
    printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T2, 1);
    while(task2_val < 2000000) {
        task2_val += 1;
        // if (task2_val % 100000 == 0) printf("Task 2\n");
    }
    // printf("[ Fin tâche 2 %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T2, 0);
    task2_val = 0;
    printf("F %d %d\n", scheduler.current_process, NOW);
}

int i = 0;
// Task 3
void task3(void) {
    printf("D %d %d\n", scheduler.current_process, NOW);
    gpio_put(LED_PIN_T3, 1);
    while (i < 1000000) {
        // if (i % 100000 == 0) printf("Task 3\n");
        i++;
    }
    gpio_put(LED_PIN_T3, 0);
    i = 0;
    printf("D %d %d\n", scheduler.current_process, NOW);
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

task_t* next_task() {
    return current_task->next;
}