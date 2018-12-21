#pragma once

#include    <core/uthread.h>
#include    <core/uthstate.h>

struct a6_uthread *uthread_self(void);
void set_uthread_self(struct a6_uthread *newself);

#define     a6_uth_entry            a6_state_entry(uthread_self())
#define     a6_uth_blocking         a6_state_blocking(uthread_self())
