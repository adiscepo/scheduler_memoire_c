#ifndef _SCHEDULER_
#define _SCHEDULER_

#include "config.h"
#include "stdint.h"
#include "stddef.h"

typedef enum {
    UNDEFINED,
    READY,
    RUNNING,
    BLOCKED,
    SUSPENDED
} state_t;

typedef struct {
    uint32_t *tos;
    uint32_t stack[PROCESS_STACK_SIZE];
    uint32_t wcet;
    uint32_t absolute_deadline;
    state_t state;
} process_t;

typedef struct {
    size_t current_process;
    process_t processes[MAX_PROCESSES];
} scheduler_t;

extern scheduler_t scheduler;
extern uint32_t tick;

void create_process(uint32_t wcet, uint32_t absolute_deadline, void(*fn)(void*));
void end_task();

// Earliest-Deadline First
size_t schedule();

#endif