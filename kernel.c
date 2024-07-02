#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/structs/systick.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "pico/stdlib.h"
#include "task.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

int task1_val, task2_val, task3_val;
static int turn = 0;
static uint32_t task1_stack[128];
// void task1();
static uint32_t task2_stack[128];
// void task2();
static uint32_t task3_stack[128];
// void task3();
void end_task(void);

// Fonctions définies en assembleur
extern void setup_systick(void);
extern void start_scheduler(void);
extern void isr_systick(void);

int main() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    stdio_init_all();
 
    // clock_configure(clk_sys,
    // CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
    // 0, // No aux mux
    // 64 * MHZ,
    // 64 * MHZ);

    // sleep_ms(2000); // Wait for communication to be established with PC for logging
    printf("[ OS ]\n");
    init_tasks();
    setup_systick();

    // add_task(1, task1, task1_stack, sizeof(task1_stack));
    // add_task(2, task2, task2_stack, sizeof(task2_stack));
    // add_task(3, task3, task3_stack, sizeof(task3_stack));
    
    current_task = &tasks[0];
    start_scheduler();

    while (true);
    return 0;
}

// Task 1
void task1(void) {
    printf("[ TASK 1 ]\n");
    while (1) {
        gpio_put(LED_PIN, 1);
        task1_val += 1;
        if (task1_val % 100000 == 0) printf("Task 1\n");
    }
}

// Task 2
void task2(void) {
    printf("[ TASK 2 ]\n");
    while(true) {
        gpio_put(LED_PIN, 0);
        task2_val += 1;
        if (task2_val % 100000 == 0) printf("Task 2\n");
    }
}

// Task 3
void task3(void) {
    printf("[ TASK 3 ]\n");
    while(true) {
        task3_val += 1;
        if (task3_val % 100000 == 0) printf("Task 3\n");
    }
}
