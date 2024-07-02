#ifndef TASK_H
#define TASK_H

#include "config.h"
#include <stdint.h>

typedef struct task {
    uint32_t *stack;
    struct task *next;
    uint32_t pid;
    uint32_t priority;
} task_t;

extern task_t *current_task;
extern task_t tasks[N_TASKS];
extern uint32_t tasks_stacks[N_TASKS][TASK_STACK_SIZE];
extern uint32_t number_threads;

void init_tasks();
void add_task(uint32_t priority, void(*fn)(void*), uint32_t* stack, uint32_t stack_size);
// Provoque un hardfault à la fin du deuxième systick_handler appelé après avoir appelé cette fonction
void end_task();

#endif
