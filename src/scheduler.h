#ifndef _SCHEDULER_
#define _SCHEDULER_

#include "config.h"
#include "stdint.h"
#include "stddef.h"

// Fonctions définies en assembleur
extern void setup_systick(void);
extern void start_scheduler(void);
extern void isr_pendsv(void);
extern void isr_systick(void);

typedef enum {
    UNDEFINED,
    DEFINED,
    READY,
    RUNNING,
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
    uint32_t delay;
} scheduler_t;

extern scheduler_t scheduler;


void init_scheduler();
int create_process(uint32_t deadline, void(*fn)(void*));
void end_task();
void idle();

// Earliest-Deadline First
size_t schedule();

#endif