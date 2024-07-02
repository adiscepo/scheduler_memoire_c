#include "task.h"

void task1();
void task2();
void task3();

void init_tasks() {
    // Critical section
    __asm("CPSID I");

    for (int i = 0; i < N_TASKS; i++) {
        if (i == N_TASKS - 1) tasks[i].next = &tasks[0];
        else tasks[i].next = &tasks[i + 1];

        if ((uint32_t)tasks[i].stack & 0x7) {
            tasks[i].stack--; // Adjust to be 8-byte aligned if necessary
        }
        tasks[i].pid = i + 1;
        tasks[i].priority = i;
        tasks[i].stack = &tasks_stacks[i][TASK_STACK_SIZE - 16]; // 16 registers
        tasks_stacks[i][TASK_STACK_SIZE - 1] = 0x01000000; // Set PSR to Thumb mode
        tasks_stacks[i][TASK_STACK_SIZE - 3] = (int32_t)(end_task); // LR: end_task est la fonction de retour
        tasks_stacks[i][TASK_STACK_SIZE - 8] = 0x44444444; // R4
        tasks_stacks[i][TASK_STACK_SIZE - 9] = 0x55555555; // R5
        tasks_stacks[i][TASK_STACK_SIZE - 10] = 0x66666666; // R6
        tasks_stacks[i][TASK_STACK_SIZE - 11] = 0x77777777; // R7
        tasks_stacks[i][TASK_STACK_SIZE - 12] = 0x88888888; // R8
        tasks_stacks[i][TASK_STACK_SIZE - 13] = 0x99999999; // R9
        tasks_stacks[i][TASK_STACK_SIZE - 14] = 0x10101010; // R10
        tasks_stacks[i][TASK_STACK_SIZE - 15] = 0x11111111; // R11
    }
    tasks_stacks[0][TASK_STACK_SIZE - 2] = (int32_t)(task1); // Set PC of task to function address
    tasks_stacks[1][TASK_STACK_SIZE - 2] = (int32_t)(task2); // Set PC of task to function address
    tasks_stacks[2][TASK_STACK_SIZE - 2] = (int32_t)(task3); // Set PC of task to function address

    current_task = &tasks[0];
    __asm("CPSIE I");
}

task_t *current_task;
task_t tasks[N_TASKS];
uint32_t tasks_stacks[N_TASKS][TASK_STACK_SIZE];
uint32_t number_threads = 0;

void add_task(uint32_t priority, void(*fn)(void*), uint32_t* stack, uint32_t stack_size) {
    __asm("CPSID I");

    task_t* new_task = &tasks[number_threads];
    stack[stack_size - 1] = 0x01000000;             // Défini le xPSR sur Thumb
    stack[stack_size - 2] = (uint32_t)fn;           // PC de la pile pointe vers le code de la fonction
    stack[stack_size - 3] = (uint32_t)&end_task;    // LR : Fonction de retour (ne fonctionne pas, mais pg car while(1) dans les tasks)
    // for (uint32_t i = 4; i < 100; i++) stack[stack_size - i] = 0x41414141;
    new_task->stack = (stack + stack_size - 16);
    new_task->pid = number_threads;
    new_task->priority = priority;

    if (number_threads > 0) {
        tasks[number_threads - 1].next = new_task; 
    }

    new_task->next = &tasks[0];         // Lie à la première tâche

    number_threads += 1;
    __asm("CPSIE I");
}

// Provoque un hardfault à la fin du deuxième systick_handler appelé après avoir appelé cette fonction
void end_task() {
    __asm("CPSID I");
    uint32_t pid = current_task->pid;
    task_t *task = &tasks[pid - 1];
    // On retire la tâche de la chaîne des tâches
    for(uint8_t i = 0; i < N_TASKS; i++) {
        if (tasks[i].next == task) {
            tasks[i].next = task->next;
            break;
        }
    }
    __asm("CPSIE I");
    while(1);
}