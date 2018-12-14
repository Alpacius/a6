#pragma once

#include    <core/uthread.h>

struct a6_uthread *current_uthread(void);

struct a6_uthread *current_limbo(void);

static
void launch_resched(void) {
    a6_uthread_switch(current_limbo(), current_uthread());
}

#define uthread_yield launch_resched
