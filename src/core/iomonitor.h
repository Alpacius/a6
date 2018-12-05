#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/list.h>
#include    <core/evadaptor_afunix.h>

struct a6_uthread;

struct a6_iomonitor;

struct a6_ioext_act {
    intrusive;
    void *arg;
    int (*hook)(struct a6_iomonitor *, void *arg);
};

#define     N_IOEXT_CHAINS          4
#define     IDX_IOEXT_PREPOLL       0
#define     IDX_IOEXT_POSTPOLL      1
#define     IDX_IOEXT_PRETIMED      1
#define     IDX_IOEXT_POSTTIMED     2
#define     IDX_IOEXT_PREIO         2
#define     IDX_IOEXT_POSTIO        3
#define     IDX_IOEXT_POSTED        3

struct a6_iomonitor {
    intrusive;
    int epfd;
    struct a6_evadaptor extevch;
    // TODO timer heap
    int cap;
    struct link_index ioext_chains[N_IOEXT_CHAINS];
    struct epoll_event epevents[];
};

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
};

#define     A6_WAITK_PLAIN   0
#define     A6_WAITK_LTERM   1

#define     a6_waitk_fd_(k_, op_) \
    ({ \
        __auto_type k__ = (k_); \
        int fd_real_ = -1; \
        switch (k__ op_ type) { \
            case A6_WAITK_PLAIN: \
                fd_real_ = k__ op_ fd.i; \
                break; \
            case A6_WAITK_LTERM: \
                fd_real_ = a6_fdwrap_fd(k__ op_ fd.w); \
                break; \
        } \
        fd_real_; \
    })
#define     a6_waitk_p_fd(k_)          a6_waitk_fd_((k_), ->)
#define     a6_waitk_o_fd(k_)          a6_waitk_fd_((k_), .)

struct a6_ioev_collector {
    intrusive;
    void (*collect)(struct a6_ioevent *, struct link_index **, uint32_t);
};

int a6_prepare_event_quick(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, uint32_t main_ev, void *udata, uint32_t options, ...);
a6_fdwrap a6_prepare_event_keepalive(struct a6_iomonitor *iomon, struct a6_uthread *uth, a6_fdwrap fdw, uint32_t main_ev, void *udata, uint32_t options, ...);
int a6_prepare_read_oneshot(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, void *udata, uint32_t options, ...);
int a6_prepare_write_oneshot(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, void *udata, uint32_t options, ...);

#define     a6_prepare_read_quick(i_, u_, f_, v_, o_, ...) \
    a6_prepare_event_quick((i_), (u_), (f_), EPOLLIN & EPOLLOUT, (v_), (o_), ##__VA_ARGS__)
#define     a6_prepare_write_quick(i_, u_, f_, v_, o_, ...) \
    a6_prepare_event_quick((i_), (u_), (f_), ~EPOLLIN & EPOLLOUT, (v_), (o_), ##__VA_ARGS__)
#define     a6_prepare_read_keepalive(i_, u_, f_, v_, o_, ...) \
    a6_prepare_event_keepalive((i_), (u_), (f_), EPOLLIN & ~EPOLLOUT, (v_), (o_), ##__VA_ARGS__)
#define     a6_prepare_write_keepalive(i_, u_, f_, v_, o_, ...) \
    a6_prepare_event_keepalive((i_), (u_), (f_), ~EPOLLIN & EPOLLOUT, (v_), (o_), ##__VA_ARGS__)

struct a6_iomonitor *a6_iomonitor_create(int cap);
void a6_iomonitor_destroy(struct a6_iomonitor *iomon);

int a6_iomonitor_poll(
        struct a6_iomonitor *iomon, 
        struct link_index **res_groups, 
        uint32_t n_res_groups, 
        void (*collect)(struct a6_ioevent *, struct link_index **, uint32_t),
        uint32_t options);
