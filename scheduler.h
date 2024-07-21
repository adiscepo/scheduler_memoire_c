#ifndef _SCHEDULER_
#define _SCHEDULER_

#include "process.h"
#include "config.h"

typedef struct {
    process_t processes[MAX_PROCESSES];
    size_t current_process;
} scheduler_t;

scheduler_t scheduler;

void interrupt() {

}

void add_process(scheduler_t &scheduler, process_t process) {
    size_t current = scheduler.current_process;
    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        if (scheduler.processes[i].state == UNDEFINED) {
            scheduler.processes[i] = process;
            if (process.absolute_deadline < scheduler.processes[current].absolute_deadline)
                scheduler.current_process = i;
        }
    }
}

// Earliest-Deadline First
size_t schedule() {
    size_t index = -1;
    uint32_t earliest_deadline = UINT32_MAX;

    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        process_t *process = &scheduler.processes[i];
        if (!process->state == state_t::RUNNING && process->absolute_deadline < earliest_deadline) {
            earliest_deadline = process->absolute_deadline;
            index = i;
        }
    }

    if (index != -1) return index;
    return -1; // TODO: Modif with IDLE process, there is nothing to schedule anymore
}

#endif