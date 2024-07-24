#include "scheduler.h"
#include "assert.h"
#include "stdio.h"
#include "pico/time.h"
// #include "task.h"

scheduler_t scheduler;
uint32_t tick = 0;


extern void set_process_idle(void);

void init_scheduler() {
    __asm("CPSID I");
    process_t* idle_process = &scheduler.processes[MAX_PROCESSES];
    idle_process->stack[PROCESS_STACK_SIZE - 1] = 0x01000000;             // Défini le xPSR sur Thumb
    idle_process->stack[PROCESS_STACK_SIZE - 2] = (uint32_t)(idle);           // PC de la pile pointe vers le code de la fonction
    idle_process->stack[PROCESS_STACK_SIZE - 3] = (uint32_t)(end_task);   // LR : Fonction de retour
    idle_process->tos = &idle_process->stack[PROCESS_STACK_SIZE - 16];      // Défini le pointeur vers le haut de la pile
    idle_process->absolute_deadline = UINT32_MAX;
    idle_process->deadline = 0;
    idle_process->state = UNDEFINED;
    scheduler.current_process = MAX_PROCESSES;
    __asm("CPSIE I");
}

void create_process(uint32_t deadline, uint32_t absolute_deadline, void(*fn)(void*)) {
    // printf("[ Création processus ] : %dms\n", NOW);
    size_t available_process = -1;
    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        if (scheduler.processes[i].state == UNDEFINED || scheduler.processes[i].state == ENDED) {
            available_process = i;
            break;
        }
    }
    assert(available_process != -1);  // TODO: modify with prettier error manager

    process_t* new_process = &scheduler.processes[available_process];
    new_process->stack[PROCESS_STACK_SIZE - 1] = 0x01000000;             // Défini le xPSR sur Thumb
    new_process->stack[PROCESS_STACK_SIZE - 2] = (uint32_t)fn;           // PC de la pile pointe vers le code de la fonction
    new_process->stack[PROCESS_STACK_SIZE - 3] = (uint32_t)(end_task);   // LR : Fonction de retour
    new_process->stack[PROCESS_STACK_SIZE - 4] = 0x12121212;             // Permet de contrôler en mémoire lors du débug
    new_process->stack[PROCESS_STACK_SIZE - 5] = 0x33333333;
    new_process->stack[PROCESS_STACK_SIZE - 6] = 0x22222222;
    new_process->stack[PROCESS_STACK_SIZE - 7] = 0x11111111;
    new_process->stack[PROCESS_STACK_SIZE - 8] = 0x00000000;
    new_process->stack[PROCESS_STACK_SIZE - 9] = 0x11111111;
    new_process->stack[PROCESS_STACK_SIZE - 10] = 0x10101010;
    new_process->stack[PROCESS_STACK_SIZE - 11] = 0x99999999;
    new_process->stack[PROCESS_STACK_SIZE - 12] = 0x88888888;
    new_process->stack[PROCESS_STACK_SIZE - 13] = 0x77777777;
    new_process->stack[PROCESS_STACK_SIZE - 14] = 0x66666666;
    new_process->stack[PROCESS_STACK_SIZE - 15] = 0x55555555;
    new_process->stack[PROCESS_STACK_SIZE - 16] = 0x44444444;
    // for (size_t i = 17; i < PROCESS_STACK_SIZE; ++i) stack[stack_size - i] = 0xdeadbeef;     // Rempli toute la pile avec des données bidons
    new_process->tos = &new_process->stack[PROCESS_STACK_SIZE - 16];      // Défini le pointeur vers le haut de la pile

    new_process->absolute_deadline = absolute_deadline;
    new_process->deadline = deadline;
    new_process->fn = (uint32_t*)fn;
    new_process->state = DEFINED;
    // printf("[ Fin création processus ] : %dms\n", NOW);
    return;
}

void end_task() {
    __asm("CPSID I");
    // On repasse en mode MSP
    __asm("mrs r0, control");
    __asm("ldr r0, =0x0");
    __asm("msr control, r0");
    __asm("isb");
    // printf("[ Fin de processus ] : %dms\n", NOW);
    size_t pid = scheduler.current_process;
    process_t *ended_process = &scheduler.processes[pid];
    ended_process->state = ENDED;
    ended_process->release_time = ended_process->absolute_deadline;
    // for (size_t i = 0; i < PROCESS_STACK_SIZE; ++i) ended_process->stack[PROCESS_STACK_SIZE - i] = 0x00000000;     // Rempli toute la pile avec des données bidons
    // ended_process->tos = 0;
    // set_process_idle();
    // create_process(ended_process->deadline, ended_process->absolute_deadline, ended_process->fn);
    // printf("[ Processus %d recréé ] : %dms\n", pid, NOW);
    // On repasse en mode PSP
    __asm("mrs r0, control");
    __asm("ldr r0, =0x2");
    __asm("msr control, r0");
    __asm("isb");
    __asm("CPSIE I");
    while (true) {
        __asm("WFI");
        // printf("[ Processus en attente de redémarrage ] : %dms\n", NOW);
    }
}


size_t schedule() {
    printf("D_CS : %dms\n", NOW);
    if (scheduler.delay == 0) {
        scheduler.delay = NOW;
        scheduler.timer = 0;
    }
    scheduler.timer =  NOW - scheduler.delay;
    // Appelé à chaque changement de contexte
    int32_t index = -1;
    uint32_t earliest_deadline = UINT32_MAX;

    for (size_t i = 0; i < MAX_PROCESSES; i++) {
        process_t *process = &scheduler.processes[i];
        if (process->state == ENDED) {
            process->stack[PROCESS_STACK_SIZE - 1] = 0x01000000;             // Défini le xPSR sur Thumb
            process->stack[PROCESS_STACK_SIZE - 2] = (uint32_t)process->fn;           // PC de la pile pointe vers le code de la fonction
            process->stack[PROCESS_STACK_SIZE - 3] = (uint32_t)(end_task);   // LR : Fonction de retour
            process->stack[PROCESS_STACK_SIZE - 4] = 0x12121212;             // Permet de contrôler en mémoire lors du débug
            process->stack[PROCESS_STACK_SIZE - 5] = 0x33333333;
            process->stack[PROCESS_STACK_SIZE - 6] = 0x22222222;
            process->stack[PROCESS_STACK_SIZE - 7] = 0x11111111;
            process->stack[PROCESS_STACK_SIZE - 8] = 0x00000000;
            process->stack[PROCESS_STACK_SIZE - 9] = 0x11111111;
            process->stack[PROCESS_STACK_SIZE - 10] = 0x10101010;
            process->stack[PROCESS_STACK_SIZE - 11] = 0x99999999;
            process->stack[PROCESS_STACK_SIZE - 12] = 0x88888888;
            process->stack[PROCESS_STACK_SIZE - 13] = 0x77777777;
            process->stack[PROCESS_STACK_SIZE - 14] = 0x66666666;
            process->stack[PROCESS_STACK_SIZE - 15] = 0x55555555;
            process->stack[PROCESS_STACK_SIZE - 16] = 0x44444444;
            // for (size_t i = 17; i < PROCESS_STACK_SIZE; ++i) stack[stack_size - i] = 0xdeadbeef;     // Rempli toute la pile avec des données bidons
            process->tos = &process->stack[PROCESS_STACK_SIZE - 16];      // Défini le pointeur vers le haut de la pile

            process->absolute_deadline = process->release_time + process->deadline;
            process->state = DEFINED;
        }
        if (process->state == DEFINED && process->release_time <= NOW - scheduler.delay) {
            printf("R_T %d : %dms\n", i, NOW);
            process->state = READY;
        }
        if (process->state == READY && process->absolute_deadline < earliest_deadline) {
            earliest_deadline = process->absolute_deadline;
            index = i;
        }
    }
    scheduler.current_process = index;
    printf("F_CS : %dms\n", NOW);
    if (index != -1) return index;
    printf("IDLE : %dms\n", NOW);
    return MAX_PROCESSES; // Retourne l'id du processus IDLE
}

void idle() {
    // while (true) printf("[ IDLE ], %dms\n", NOW);
    while (true) __asm("NOP");
}