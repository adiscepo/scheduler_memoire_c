#ifndef CONFIG_H
#define CONFIG_H

#define NOW to_ms_since_boot(get_absolute_time()) - scheduler.delay

#define TASK_STACK_SIZE 1024 // 1KB
#define PROCESS_STACK_SIZE 1024 // 1KB

#define MAX_PROCESSES 3

#endif