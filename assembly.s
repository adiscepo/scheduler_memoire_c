.syntax unified
.cpu cortex-m0
.fpu softvfp

.thumb

.equ SYSTICK_BASE, 0xe000e010   // 0 : CSR
                                // 4 : RVR
                                // 8 : CVR
                                // C : CALIB -> Pas important dans notre cas
.equ SYSTICK_RELOAD_VALUE, 0x00ffffff - 1  // Valeur de Reload pour Systick (valeur maximale)
.equ SYSTICK_SHPR3, 0xe000ed20
.equ SYSTICK_PRIORITY_MASK, 0xc0000000
.equ SYSTICK_PRIORITY, 3

// J'aurais pu utiliser des fonctions proposées par le sdk du rpi pico (pour définir les
// exceptions, les valeurs de systick, etc.) mais j'ai préféré les faire en assembleur
// afin de saisir la substantifique moelle du fonctionnement de la puce
.global setup_systick
.type setup_systick, %function
setup_systick:
    cpsid i
    
    ldr  r1, =SYSTICK_BASE  // Charge le registre de gestion de Systick (pg. 78 ref RP2040)
    movs r0, #0             // Réinitialise les registres
    str  r0, [r1]           // CSR (Control and Status Register)
    str  r0, [r1, #4]       // RVR (Reload Value Register)
    str  r0, [r1, #8]       // CVR (Current Value Register)
    
    ldr  r0, =SYSTICK_RELOAD_VALUE
    str  r0, [r1, #4]       // Défini la valeur de reload sur SYSTICK_RELOAD_VALUE

    movs r0, #0x07          // Active les bits 0, 1 et 2 du registre CSR (pg. 101 ref ARMCortexM0+)
    str  r0, [r1]

    ldr  r1, =SYSTICK_SHPR3
    ldr  r2, [r1]
    ldr  r0, =SYSTICK_PRIORITY_MASK
    bics r2, r0
    movs r0, #SYSTICK_PRIORITY       // Défini la priorité de PendSV à basse (3) et Systick à élevée (0)
    lsls r0, r0, 30
    orrs r2, r0
    str  r2, [r1]
    
    cpsie i
    bx lr

.global start_scheduler
.type start_scheduler, %function
start_scheduler:
    CPSID i
    ldr r0, =current_task   // r0 = &current_task
    ldr r1, [r0]            // r1 = current_task
    ldr r2, [r1]            // r2 = current_task->stack_ptr
    mov sp, r0              // Place le pointeur de la pile de tâche en tant que sp
    pop {r4-r7}             // Récupère le contexte de la pile
    pop {r4-r7}
    pop {r0-r3}
    pop {r4-r5}
    pop {r3}
    mov lr, r3
    pop {r3}
    cpsie i
    bx  lr

.global isr_systick
.type isr_systick, %function
isr_systick:
    cpsid i

    push {r4-r7}    // Sauve le contexte actuel 
    mov r4, r8      // On doit passer par deux push successif car on est en mode Thumb
    mov r5, r9
    mov r6, r10
    mov r7, r11
    push {r4-r7}

    mov r2, sp              // Sauve le sp courant (Redondant avec le mov sp, r2)
    ldr r1, =current_task   // r1 = &current_task
    ldr r0, [r1]            // current_task->stack = &sp
    str r2, [r0]

    ldr r2, [r0, #4]        // r2 = current_task->next
    str r2, [r1]            // current_task->next = r1
    ldr r0, [r2]
    mov sp, r0

    pop {r4-r7}             // Restaure la pile
    mov r8, r4
    mov r9, r5
    mov r10, r6
    mov r11, r7
    pop {r4-r7}

    cpsie i
    bx lr
