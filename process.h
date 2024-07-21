#ifndef _PROCESS_
#define _PROCESS_

#include "task.h"

typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    SUSPENDED,
    UNDEFINED
} state_t;

typedef struct {
    uint32_t wcet;
    uint32_t absolute_deadline;
    task_t* task;
    state_t state = UNDEFINED;
} process_t;

process_t create_process(uint32_t wcet, uint32_t absolute_deadline, task_t *task) {
    process_t proc;
    proc.absolute_deadline = absolute_deadline;
    proc.wcet = wcet;
    proc.task = task;
    proc.state = READY;
}

#endif