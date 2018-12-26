#pragma once

#include    <core/uthread.h>

struct a6_uthread *current_uthread(void);

struct a6_uthread *current_limbo(void);

static
void launch_resched(void) {
    a6_uthread_switch(current_limbo(), current_uthread());
}

static
void blocking_resched(void) {
    struct a6_uthread *f = current_uthread();
    list_del(intrusion_from_ptr(f));
    a6_uthread_switch(current_limbo(), f);
}

#define uthread_yield blocking_resched()
