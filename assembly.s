.syntax unified
.cpu cortex-m0
.fpu softvfp

.thumb

.equ SYSTICK_BASE, 0xe000e010               // 0 : CSR
                                            // 4 : RVR
                                            // 8 : CVR
                                            // C : CALIB -> Pas important dans notre cas
.equ SYSTICK_RELOAD_VALUE, 0x00ffffff - 1   // Valeur de Reload pour Systick (valeur maximale)
.equ SYSTICK_SHPR3, 0xe000ed20
.equ SYSTICK_PRIORITY_MASK, 0xc0000000
.equ SYSTICK_PRIORITY, 3

.equ PENDSV_BASE, 0xe000ed04

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

    ldr  r1, =SYSTICK_SHPR3         // Charge l'adresse permettant de gérer les priorités des interruptions
    ldr  r2, [r1]
    ldr  r0, =SYSTICK_PRIORITY_MASK // Défini le masque de priorité afin de cibler l'interruption Systick
    bics r2, r0
    movs r0, #SYSTICK_PRIORITY      // Défini la priorité de PendSV à basse (3) et Systick à élevée (0)
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
    
    // Bascule en mode Thread
    @ mov sp, r2             // Place le pointeur de la pile de tâche en tant que psp
    msr psp, r2             // Place le pointeur de la pile de tâche en tant que psp
    isb
    mrs r0, control
    ldr r0, =#0x2          // Défini le 2ème bit de CONTROL à 1 -> Passe en mode thread 
    msr control, r0
    isb

    ldr r3, [r2, #56]         // Récupère l'adresse d'entrée de la fonction, le contenu de la stack n'est pas nécessaire à récupérer car bidon
    @ pop {r4-r7}             // Récupère le contexte de la pile
    @ pop {r4-r7}
    @ pop {r0-r3}
    @ pop {r0-r1}
    @ pop {r3}
    mov lr, r3                // Récupère l'adresse de la fonction à exécuter (se trouve en stack_size - 3)
    @ pop {r3}
    cpsie i
    bx  lr                  // Branchement vers la fonction référencée dans la pile

.global isr_systick
.type isr_systick, %function
isr_systick:
    cpsid i
    
    ldr r1, =PENDSV_BASE    // Charge le registre de gestion des interruptions (pg. 85 rp2040)
    ldr r0, =0x10000000     // Met le 28ème bit à 1 -> Active l'interruption PendSV
    str r0, [r1]

    cpsie i
    bx lr

.global isr_pendsv
.type isr_pendsv, %function
isr_pendsv:
    cpsid i

    mrs r0, psp             // Sauve le pointeur de pile de la tâche actuelle
    subs r0, #32            // Fait de l'espace afin de sauver les registres r4 à r11 (32 octets = 8 registres)
    stmia r0!, {r4-r7}      // Sauve le contexte de la tâche sur sa propre pile 
    mov r4, r8
    mov r5, r9
    mov r6, r10
    mov r7, r11
    stmia r0!, {r4-r7}      // Sauve les registres r8-r11
    subs r0, #32            // On ramène le pointeur de pile au bon endroit (il a été modifié par l'opération stmia)

    ldr r1, =current_task   // r1 = &current_task
    ldr r2, [r1]            // r2 = current_task
    str r0, [r2]            // current_task->stack = r0 (PSP)

    // Calcul de la tâche suivante
    mov r3, lr              // Récupère la valeur de lr avant d'entrer dans la fonction
    push {r3}               // Sauve lr sur la pile
    bl next_task            // Appel à la fonction de calcul de tâche suivante (stockée dans r0)
    str r0, [r1]            // Défini la tâche suivante comme tâche courante (r1 = current_task)
    ldr r0, [r0]            // Récupère le pointeur de pile de la tâche
    pop {r3}                // Récupère la valeur de lr stockée sur la pile
    mov lr, r3

    @ ldr r2, [r2, #4]        // r2 = current_task->next
    @ str r2, [r1]            // current_task = current_task->next
    @ ldr r0, [r2]            // r0 = next_task->stack

    adds r0, #16            // On place le curseur de pile sur les registres r8 à r11
    ldmia r0!, {r4-r7}      // On restore les registres r8 à r11
    mov r8, r4
    mov r9, r5
    mov r10, r6
    mov r11, r7
    subs r0, #32            // On remet le curseur de pile sur les registres r4 à r7
    ldmia r0!, {r4-r7}      // On restore les registres r4 à r7
    adds r0, #16            // On remet le pointeur de la pile à la bonne place (celle où se trouvent les registres sauvés automatiquement)
    msr psp, r0             // Met à jour le PSP avec le pointeur de pile de la tâche suivante

    cpsie i
    bx lr


.data
    .align  4
