#pragma once

#include    <stddef.h>
#include    <stdint.h>

struct a6_uthread;

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

struct a6_ioevent {
    int type;
    int fd;
    void *udata;
};

#define     A6_IOEV_EP       0
#define     A6_IOEV_TO       1

struct a6_waitk {
    intrusive;
    int type;
    union {
        a6_fdwrap w;
        int i;
    } fd;
    uint32_t evres;
    struct a6_uthread *uth;
};

#define     A6_WAITK_DUMMY  -1
#define     A6_WAITK_PLAIN   0
#define     A6_WAITK_LTERM   1

#define     A6_FD_DUMMY     -1

#define     a6_waitk_fd_(k_, op_) \
    ({ \
        struct a6_waitk *k__ = (k_); \
        int fd_real_ = -1; \
        switch (k__ op_ type) { \
            case A6_WAITK_PLAIN: \
                fd_real_ = k__ op_ fd.i; \
                break; \
            case A6_WAITK_LTERM: \
                fd_real_ = a6_fdwrap_fd(k__ op_ fd.w); \
                break; \
            default: \
                fd_real_ = A6_FD_DUMMY; \
        } \
        fd_real_; \
    })
#define     a6_waitk_p_fd(k_)          a6_waitk_fd_((k_), ->)
#define     a6_waitk_o_fd(k_)          a6_waitk_fd_((k_), .)

