#pragma once

#ifndef     __x86_64__
#error      "Wrong architecture: x86-64 expected."
#endif

#ifdef      a6_mcontext
#error      "a6_mcontext already defined."
#endif

#define     a6_mcontext                a6_mcontext_x64

#define     a6_mcontext_init           a6_mcontext_x64_init
#define     a6_mcontext_switch         a6_mcontext_x64_switch
#define     a6_mcontext_stack_base     a6_mcontext_x64_stack_base

#include    <common/stdc_common.h>

/*
 * Userland CPU context for x86-64/linux.
 *
 * There's no going back - the only way in & out is the function initially called and its way out is to crash.
 * It is up to the wrapper to decide how to escape since no context could easily self-destruct.
 */

struct a6_mcontext_x64 {
    uint64_t rdi, rsi, rdx, rcx, r8, r9, rax, rbx, r10;
    uint64_t rsp, rbp;
    uint64_t r11, r12, r13, r14, r15;
    uint64_t rip;
} __attribute__((packed));

void a6_mcontext_x64_init(struct a6_mcontext_x64 *mcontext, void (*entrance)(void *), void *argument, void *stack_base_real);
void a6_mcontext_x64_switch(struct a6_mcontext_x64 *to, struct a6_mcontext_x64 *from);

static inline
void *a6_mcontext_x64_stack_base(void *stack_base, size_t stack_size) {
    return stack_base + stack_size - 2 * sizeof(void *);
}
