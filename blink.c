#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <stdint.h>

extern int add(int a, int b);

const uint LED_PIN = PICO_DEFAULT_LED_PIN;


bool repeating_timer_callback(struct repeating_timer *t) {
    static int count = 0;
    int a = 23;
    int b = 42;
    printf("Timer fired %d times\n", ++count);
    printf("C : %d\n", add(a, b));
    return true;
}


void kernel_main(void) {

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    stdio_init_all();

    struct repeating_timer timer;
    add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}

int main() {
    kernel_main();
    return 0;
}