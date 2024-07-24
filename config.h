#ifndef CONFIG_H
#define CONFIG_H

#define NOW to_ms_since_boot(get_absolute_time()) - scheduler.delay
// #define RAM_START 0x20000000
// #define RAM_SIZE  (32 * 1024) // 32KB

// #define KERNEL_STACK (RAM_START + RAM_SIZE) // Position de la stack utilisée par le kernel (MSP)
#define TASK_STACK_SIZE 1024 // 1KB
#define PROCESS_STACK_SIZE 1024 // 1KB

#define N_TASKS 3
#define MAX_PROCESSES 10

#endif