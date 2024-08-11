#include "scheduler.h"
#include "assert.h"
#include "stdio.h"
#include "pico/time.h"
#include "hardware/irq.h"

#define END_TASK_IRQ 16

scheduler_t scheduler;
uint32_t tick = 0;

extern void set_process_idle(void);
extern void end_set_task(void);

void setup_end_task_irq()
{
    irq_set_exclusive_handler(END_TASK_IRQ, end_set_task);
    irq_set_enabled(END_TASK_IRQ, false);
    irq_set_priority(END_TASK_IRQ, 10);
}

void end_set_task_irq()
{
    irq_set_enabled(END_TASK_IRQ, true);
}

void init_scheduler()
{
    __asm("CPSID I");
    setup_systick();      // Lance la configuration de SysTick
    setup_end_task_irq(); // Active l'interruption de fin de tâche
    process_t *idle_process = &scheduler.processes[MAX_PROCESSES];
    idle_process->stack[PROCESS_STACK_SIZE - 1] = 0x01000000;           // Défini le xPSR sur Thumb
    idle_process->stack[PROCESS_STACK_SIZE - 2] = (uint32_t)(idle);     // PC de la pile pointe vers le code de la fonction
    idle_process->stack[PROCESS_STACK_SIZE - 3] = (uint32_t)(end_task); // LR : Fonction de retour
    idle_process->tos = &idle_process->stack[PROCESS_STACK_SIZE - 16];  // Défini le pointeur vers le haut de la pile
    idle_process->absolute_deadline = UINT32_MAX;
    idle_process->deadline = UINT32_MAX;
    idle_process->state = UNDEFINED;
    scheduler.current_process = MAX_PROCESSES;
    __asm("CPSIE I");
}

int create_process(uint32_t deadline, void (*fn)(void *))
{
    size_t available_process = -1;
    for (size_t i = 0; i < MAX_PROCESSES; i++)
    {
        if (scheduler.processes[i].state == UNDEFINED || scheduler.processes[i].state == ENDED)
        {
            available_process = i;
            break;
        }
    }
    if (available_process == -1)
        return false; // Plus d'espace pour de nouveaux processus

    process_t *new_process = &scheduler.processes[available_process];
    new_process->stack[PROCESS_STACK_SIZE - 1] = 0x01000000;           // Défini le xPSR sur Thumb
    new_process->stack[PROCESS_STACK_SIZE - 2] = (uint32_t)fn;         // PC de la pile pointe vers le code de la fonction
    new_process->stack[PROCESS_STACK_SIZE - 3] = (uint32_t)(end_task); // LR : Fonction de retour
    new_process->stack[PROCESS_STACK_SIZE - 4] = 0x12121212;           // Permet de contrôler en mémoire lors du débug
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
    new_process->tos = &new_process->stack[PROCESS_STACK_SIZE - 16]; // Défini le pointeur vers le haut de la pile

    new_process->absolute_deadline = deadline;
    new_process->deadline = deadline;
    new_process->fn = (uint32_t *)fn;
    new_process->state = DEFINED;
    return true;
}

void end_task()
{
    __asm("CPSID I");
    size_t pid = scheduler.current_process;
    process_t *ended_process = &scheduler.processes[pid];
    ended_process->state = ENDED;
    __asm("CPSIE I");
    end_set_task_irq();
    // On est pas censé être ici, l'interruption précédente nous emmène vers la tâche suivante directe
    while (true)
    {
        __asm("WFI");
    }
}

size_t schedule()
{
    __asm("CPSID I");

    if (scheduler.delay == 0)
    {
        scheduler.delay = NOW;
    }
    else
    {
        debug_printf("P %d %d\n", scheduler.current_process, NOW);
        debug_printf("D %d %d\n", MAX_PROCESSES + 1, NOW);
    }

    // if (NOW >= 10000)
    //     __asm("bkpt #0");

    int32_t pid = MAX_PROCESSES;
    uint32_t earliest_deadline = UINT32_MAX;

    for (size_t i = 0; i < MAX_PROCESSES; ++i)
    {
        process_t *process = &scheduler.processes[i];

        if (process->state == UNDEFINED)
            continue;

        if (process->state == ENDED)
        {
            process->stack[PROCESS_STACK_SIZE - 1] = 0x01000000;            // xPSR
            process->stack[PROCESS_STACK_SIZE - 2] = (uint32_t)process->fn; // PC
            process->stack[PROCESS_STACK_SIZE - 3] = (uint32_t)(end_task);  // LR
            process->stack[PROCESS_STACK_SIZE - 4] = 0x12121212;            // R12
            process->stack[PROCESS_STACK_SIZE - 5] = 0x33333333;            // R3
            process->stack[PROCESS_STACK_SIZE - 6] = 0x22222222;            // R2
            process->stack[PROCESS_STACK_SIZE - 7] = 0x11111111;            // R1
            process->stack[PROCESS_STACK_SIZE - 8] = 0x00000000;            // R0
            process->stack[PROCESS_STACK_SIZE - 9] = 0x11111111;            // R11
            process->stack[PROCESS_STACK_SIZE - 10] = 0x10101010;           // R10
            process->stack[PROCESS_STACK_SIZE - 11] = 0x99999999;           // R9
            process->stack[PROCESS_STACK_SIZE - 12] = 0x88888888;           // R8
            process->stack[PROCESS_STACK_SIZE - 13] = 0x77777777;           // R7
            process->stack[PROCESS_STACK_SIZE - 14] = 0x66666666;           // R6
            process->stack[PROCESS_STACK_SIZE - 15] = 0x55555555;           // R5
            process->stack[PROCESS_STACK_SIZE - 16] = 0x44444444;           // R4
            process->tos = &process->stack[PROCESS_STACK_SIZE - 16];        // TOS
            process->state = DEFINED;
            process->release_time = process->absolute_deadline;
            process->absolute_deadline = process->release_time + process->deadline;
        }

        if (process->state == DEFINED || process->state == FAILED)
        {
            if (process->release_time <= NOW)
            {
                process->state = READY;
                debug_printf("R %d %d\n", i, NOW);
            }
        }

        if (process->absolute_deadline < NOW)
        {
            debug_printf("M %d %d\n", i, NOW);
            process->state = FAILED;
            __breakpoint();
        }

        if (process->state == RUNNING)
        {
            process->state = PREEMPTED;
        }

        if (process->state == READY || process->state == RUNNING || process->state == PREEMPTED)
        {
            if (process->absolute_deadline <= earliest_deadline)
            {
                if (process->deadline < scheduler.processes[pid].deadline) { // En cas d'égalité, on prend la tâche qui a la plus petite période
                    pid = i;
                    earliest_deadline = process->absolute_deadline;
                }
            }
        }
    }

    if (pid != MAX_PROCESSES)
    {
        scheduler.processes[pid].state = RUNNING;
    }
    scheduler.current_process = pid;

    debug_printf("F %d %d\n", MAX_PROCESSES + 1, NOW);
    if (pid == MAX_PROCESSES)
        debug_printf("IDLE %d\n", NOW);
    __asm("CPSIE I");
    return pid;
}

void idle()
{
    while (true)
        __asm("NOP");
}