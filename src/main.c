#include "task.h"
#include <stdio.h>

int main() {
    int32_t stack[100];
    task_t t;
    t.stack = &stack;
    t.next = NULL;

    printf("Size of task : %lu\n", sizeof(t));
}