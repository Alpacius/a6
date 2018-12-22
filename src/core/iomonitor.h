#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/list.h>
#include    <core/evadaptor_afunix.h>
#include    <core/ioext.h>
#include    <core/waitk.h>

struct a6_uthread;

struct a6_iomonitor;

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
    struct {
        int timeout;
    } current_state;
    struct link_index ioext_chains[N_IOEXT_CHAINS];
    struct epoll_event epevents[];
};

struct a6_ioev_collector {
    intrusive;
    void (*collect)(struct a6_ioevent *, struct link_index *, uint32_t);
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

void a6_attach_ioext_hook(struct a6_iomonitor *iomon, struct a6_ioext_act *act, uint32_t timing);
