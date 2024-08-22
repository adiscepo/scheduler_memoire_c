#ifndef CONFIG_H
#define CONFIG_H

#define NOW to_ms_since_boot(get_absolute_time()) - scheduler.delay

#define TASK_STACK_SIZE 1024 // 1KB
#define PROCESS_STACK_SIZE 1024 // 1KB

#define MAX_PROCESSES 3

#ifdef DEBUG
    #define debug_printf(...) printf(__VA_ARGS__)
#else
    #define debug_printf(...)
#endif

#endif