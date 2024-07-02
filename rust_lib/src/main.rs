#![no_std]
#![no_main]

use cortex_m::asm;
use rp_pico::entry;
use panic_halt as _;
use rp_pico::hal;
use rp_pico::hal::pac;
use rp_pico::hal::prelude::*;
use embedded_hal::digital::OutputPin;

const N_TASKS: usize = 2;
const TASK_STACK_SIZE: usize = 100;

struct Task {
    stack: [u32; TASK_STACK_SIZE],
    next: usize,
}

static mut TASKS: [Task; N_TASKS] = [Task { stack: [0; TASK_STACK_SIZE], next: 0 }; N_TASKS];
static mut CURRENT_TASK: usize = 0;


#[no_mangle]
extern "C" fn task1() {
    loop {
        // Task 1 code here
    }
}

#[no_mangle]
extern "C" fn task2() {
    loop {
        // Task 2 code here
    }
}


#[entry]
fn main() -> ! {
    // Initialization code here

    unsafe {
        // Initialize tasks
        for i in 0..N_TASKS {
            TASKS[i].next = if i == N_TASKS - 1 { 0 } else { i + 1 };
            TASKS[i].stack[TASK_STACK_SIZE - 1] = 0x01000000; // Set PSR to Thumb mode
            TASKS[i].stack[TASK_STACK_SIZE - 2] = match i {
                0 => task1 as u32,
                _ => task2 as u32,
            };
        }

        // Set the SysTick exception priority
        let mut scb = pac::CorePeripherals::take().unwrap().SCB;
        scb.set_priority(pac::Interrupt::SysTick, 0x00);

        // Enable the SysTick exception
        let mut syst = pac::CorePeripherals::take().unwrap().SYST;
        syst.set_clock_source(cortex_m::peripheral::syst::SystClkSource::Core);
        syst.set_reload(0xFFFFFF);
        syst.clear_current();
        syst.enable_counter();
        syst.enable_interrupt();
    }

    loop {
        // Main loop
    }
}

#[no_mangle]
pub unsafe extern "C" fn SysTick() {
        // Save context
        asm!("
            mrs r0, psp
            stmdb r0!, {r4-r11}
            msr psp, r0
        " :::: "volatile");

        // Switch tasks
        CURRENT_TASK = TASKS[CURRENT_TASK].next;

        // Restore context
        asm!("
            mrs r0, psp
            ldmia r0!, {r4-r11}
            msr psp, r0
        ");
}
