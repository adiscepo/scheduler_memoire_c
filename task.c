#include "task.h"
#include "pico/stdlib.h"

task_t *current_task;
task_t tasks[N_TASKS];
uint32_t number_threads = 0;

// void add_task(uint32_t priority, void(*fn)(void*), uint32_t* stack, uint32_t stack_size) {
//     __asm("CPSID I");
//     task_t* new_task = &tasks[number_threads];
//     stack[stack_size - 1] = 0x01000000;             // Défini le xPSR sur Thumb
//     stack[stack_size - 2] = (uint32_t)fn;           // PC de la pile pointe vers le code de la fonction
//     stack[stack_size - 3] = (uint32_t)(end_task);   // LR : Fonction de retour (ne fonctionne pas, mais pg car while(1) dans les tasks)
//     stack[stack_size - 4] = 0x12121212;             // Permet de contrôler en mémoire lors du débug
//     stack[stack_size - 5] = 0x33333333;
//     stack[stack_size - 6] = 0x22222222;
//     stack[stack_size - 7] = 0x11111111;
//     stack[stack_size - 8] = 0x00000000;
//     stack[stack_size - 9] = 0x11111111;
//     stack[stack_size - 10] = 0x10101010;
//     stack[stack_size - 11] = 0x99999999;
//     stack[stack_size - 12] = 0x88888888;
//     stack[stack_size - 13] = 0x77777777;
//     stack[stack_size - 14] = 0x66666666;
//     stack[stack_size - 15] = 0x55555555;
//     stack[stack_size - 16] = 0x44444444;
//     // for (size_t i = 17; i < stack_size; ++i) stack[stack_size - i] = 0xdeadbeef;     // Rempli toute la pile avec des données bidons
//     new_task->stack = &stack[stack_size - 16];      // Défini le pointeur vers le haut de la pile
//     new_task->pid = number_threads;
//     new_task->priority = priority;

//     if (number_threads > 0) tasks[number_threads - 1].next = new_task; 
//     new_task->next = &tasks[0];         // Lie à la première tâche

//     number_threads += 1;
//     __asm("CPSIE I");
// }

// void end_task() {
//     __asm("CPSID I");
//     uint32_t pid = current_task->pid;

//     task_t *task = &tasks[pid];
//     // On retire la tâche de la chaîne des tâches
//     for(uint8_t i = 0; i < N_TASKS; i++) {
//         if (tasks[i].next == task) {
//             tasks[i].next = task->next;
//             break;
//         }
//     }
//     __asm("CPSIE I");
//     while(1);
// }

void select_next_task() {
    current_task = current_task->next;
}