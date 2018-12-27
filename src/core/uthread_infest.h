#pragma once

#include    <core/uthread.h>

struct a6_uthread *current_limbo(void);
struct a6_uthread *current_uthread(void);

void mark_self_blocking(void);


static
void blocking_resched(void) {
    struct a6_uthread *f = current_uthread();
    list_del(intrusion_from_ptr(f));
    mark_self_blocking();
    a6_uthread_switch(current_limbo(), f);
}

#define uthread_yield blocking_resched()
