#pragma once

#include    <stddef.h>
#include    <stdint.h>
#include    <common/list.h>
#include    <core/fdwrap.h>

struct a6_uthread;

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
            case A6_WAITK_DUMMY: \
            default: \
                fd_real_ = A6_FD_DUMMY; \
        } \
        fd_real_; \
    })
#define     a6_waitk_p_fd(k_)          a6_waitk_fd_((k_), ->)
#define     a6_waitk_o_fd(k_)          a6_waitk_fd_((k_), .)

