#pragma once

#include    <common/stdc_common.h>
#include    <common/miscutils.h>
#include    <common/list.h>
#include    <core/mcontext.h>
#include    <core/uthstate.h>

struct a6_scheduler;

struct a6_uthread {
    intrusive;
    struct {
        struct a6_uthread *cont;
        struct a6_mcontext m;
        void (*entrance)(void *);
        void *arg;
        void *stkbase;
        size_t stksize;
    } k;
    struct link_index states;
    struct a6_scheduler *sched;
};

#define     a6_uthreadization_(n_)      struct a6_uthread n_
#define     a6_uth_dummyvar_name        a6_uth_dummy
#define     a6_uthreadization           a6_uthreadization_(a6_uth_dummyvar_name)

#define     a6_ext2uth_p(x_)            (&((x_)->a6_uth_dummyvar_name))
#define     a6_ext2uth_i(x_)            (&((x_).a6_uth_dummyvar_name))
#define     a6_ext2uth(x_)              a6_ext2uth_p((x_))
#define     a6_uth_host(u_, t_)         container_of((u_), t_, a6_uth_dummyvar_name)

// yeah sugar
#define     a6_stated_scope

static inline
void a6_statevar_cleanup(struct a6_uthstate *arg) {
    list_del(intrusion_from_ptr(arg));
}

#define     a6_statevar_name            a6localstvar_
#define     a6_state_insertion(sn_, u_) \
    struct a6_uthstate a6_statevar_name \
        __attribute__((cleanup(a6_statevar_cleanup))) = { .state = (sn_) }; \
    do { \
        struct a6_uthread *u__ = (u_); \
        list_add_head(intrusion_from_imm(a6_statevar_name), &(u__->states)); \
    } while (0)

#define     a6_state_entry(u_)          a6_state_insertion(A6_UTH_ENTRY, (u_))
#define     a6_state_blocking(u_)       a6_state_insertion(A6_UTH_BLOCKING, (u_))

void a6_uthread_init(struct a6_uthread *uth, void (*entrance)(void *), void *arg, void *stkbase, size_t stksize);
void a6_uthread_ruin(struct a6_uthread *uth);

struct a6_uthread *a6_uthread_create(void (*entrance)(void *), void *arg, size_t stknpages);
void a6_uthread_destroy(struct a6_uthread *uth);

void a6_uthread_switch(struct a6_uthread *to, struct a6_uthread *from);
int a6_uthread_launch(struct a6_uthread *target, struct a6_uthread *cont);
