#pragma once

#include    <common/stdc_common.h>
#include    <ucontext.h>

#ifdef      a6_mcontext
#error      a6_mcontext already defined.
#endif

#define     a6_mcontext                 ucontext_t

#define     a6_mcontext_init            a6_mcontext_posix_init
#define     a6_mcontext_switch          a6_mcontext_posix_switch
#define     a6_mcontext_stack_base      a6_mcontext_posix_stack_base

static inline
void a6_mcontext_posix_init(ucontext_t *mcontext, void (*entrance)(void *), void *argument, void *stack_base_real, size_t stack_size) {
    mcontext->uc_link = NULL;
    (mcontext->uc_stack.ss_sp = stack_base_real), (mcontext->uc_stack.ss_size = stack_size);
    getcontext(mcontext);
    makecontext(mcontext, (void (*)()) entrance, 1, argument);
}

static inline
void a6_mcontext_posix_switch(ucontext_t *to, ucontext_t *from) {
    swapcontext(from, to);
}

static inline
void *a6_mcontext_posix_stack_base(void *stack_base, size_t stack_size) {
    return stack_base;
}
