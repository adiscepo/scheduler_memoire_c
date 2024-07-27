#ifndef _SCHEDULER_
#define _SCHEDULER_

#include "config.h"
#include "stdint.h"
#include "stddef.h"

typedef enum {
    UNDEFINED,
    DEFINED,
    READY,
    RUNNING,
    BLOCKED,
    FAILED,
    PREEMPTED,
    ENDED
} state_t;

typedef struct {
    uint32_t *tos;
    uint32_t stack[PROCESS_STACK_SIZE];
    uint32_t deadline;
    uint32_t absolute_deadline;
    uint32_t release_time;
    uint32_t *fn;
    state_t state;
} process_t;

typedef struct {
    size_t current_process;
    process_t processes[MAX_PROCESSES + 1];
    uint32_t timer;
    uint32_t delay;
} scheduler_t;

extern scheduler_t scheduler;
extern uint32_t tick;


void init_scheduler();
void create_process(uint32_t deadline, uint32_t absolute_deadline, void(*fn)(void*));
void end_task();
void idle();

// Earliest-Deadline First
size_t schedule();

#endif