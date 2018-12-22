#include    <stdio.h>
#include    <core/uthread.h>

struct a6_uthread *first, *second, *root;

void uth_entry_first(void *raw) {
    const char *str = raw;
    printf("first_in=%s\n", str);
    a6_uthread_switch(root, first);
    printf("first_out=%s\n", str);
}

void uth_entry_second(void *raw) {
    const char *str = raw;
    printf("second_in=%s\n", str);
    a6_uthread_switch(root, second);
    printf("second_out=%s\n", str);
}

int main(void) {
    struct a6_uthread rootflow;
    root = &rootflow;
    first = a6_uthread_create(uth_entry_first, "firstarg", 32);
    second = a6_uthread_create(uth_entry_second, "secondarg", 32);
    a6_uthread_launch(first, root);
    a6_uthread_launch(second, root);
    a6_uthread_switch(first, root);
    a6_uthread_switch(second, root);
    a6_uthread_destroy(first);
    a6_uthread_destroy(second);
    return 0;
}
