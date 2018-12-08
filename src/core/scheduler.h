#pragma once

#include    <common/stdc_common.h>
#include    <common/list.h>
#include    <common/miscutils.h>
#include    <core/evadaptor_afunix.h>
#include    <core/ioext.h>

struct a6_iomonitor;

#define     N_IOEXT_ACTS        2

struct a6_scheduler {
    intrusive;
    struct link_index running, blocking, dying;
    struct a6_iomonitor *iomon;
    struct {
        uint64_t max_n_uth;
    } baseinfo;
    struct {
        struct link_index queue;
        pthread_spinlock_t lock;
    } qreqs;
    struct a6_ioext_act ioext_hooks[N_IOEXT_ACTS];
    struct a6_evadaptor evchan;
};

struct a6_uthread;

struct a6_asynck_arg {
    void (*dtor_hook)(struct a6_asynck_arg *);
};

// TODO intrusive interface facilities

struct a6_asynck {
    struct a6_uthread *uth;
    int (*kfunc)(struct a6_asynck *);
    struct a6_asynck_arg *karg;
};

void schedloop(struct a6_scheduler *s);
