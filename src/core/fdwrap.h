#pragma once

#include    <stddef.h>
#include    <stdint.h>

#ifdef      __x86_64__

typedef uint64_t a6_fdwrap;

#define     FDWRAP_X86_64_OPTMASK           0xffffffff00000000
#define     FDWRAP_X86_64_FDMASK            0x00000000ffffffff

#define     FDWRAP_X86_64_EXISTS            0x1
#define     FDWRAP_X86_64_ERROR             0x2

#define     a6_fdwrap_init(i_)              ((i_) & FDWRAP_X86_64_FDMASK)
#define     a6_fdwrap_new(i_, o_)           (a6_fdwrap_init((i_))|(((uint64_t) (o_)) << 32))
#define     a6_fdwrap_fd(w_)                ((int) ((w_) & FDWRAP_X86_64_FDMASK))
#define     a6_fdwrap_opt(w_)               ((uint32_t) (((w_) & FDWRAP_X86_64_OPTMASK) >> 32))

#define     a6_fdwrap_check(w_, o_)         (a6_fdwrap_opt((w_)) & (o_))
#define     a6_fdwrap_exists(w_)            a6_fdwrap_check((w_), FDWRAP_X86_64_EXISTS)

#define     a6_fdwrap_with_opt(w_, o_)      ((w_)|(((uint64_t) (o_)) << 32))
#define     a6_fdwrap_without_opt(w_, o_)   ((w_) & ~((uint64_t) (o_) << 32))

#define     a6_fdwrap_mark_reg(w_)          a6_fdwrap_with_opt((w_), FDWRAP_X86_64_EXISTS)
#define     a6_fdwrap_mark_err(w_)          a6_fdwrap_with_opt((w_), FDWRAP_X86_64_ERROR)

#else       // __x86_64__

typedef struct {
    int fd;
    int options;
} a6_fdwrap;

#endif      // __x86_64__

