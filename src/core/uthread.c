#include    <core/uthread.h>
#include    <core/uthreq.h>

void a6i_mark_dying(struct a6_uthread *uth);
struct a6_uth_req a6i_req_steal(struct a6_scheduler *sched);

struct a6_uthread *current_limbo(void);
void mark_self_alive(struct a6_uthread *uth);

static
void a6_uthread_entrance(void *uthp) {
    struct a6_uthread *uth = uthp;
    a6_stated_scope {
        a6_state_entry(uth);
        uth->k.entrance(uth->k.arg);
    }
#ifdef A6_ENABLE_UTH_REUSE
    int reuse = 1;
    uint32_t extralives = 2;
    struct a6_scheduler *sched = uth->sched;
    while (reuse && --extralives) {
        reuse = 0;
        struct a6_uth_req ur = a6i_req_steal(sched);
        if (ur.func) {
            (reuse = 1), (uth->k.entrance = ur.func), (uth->k.arg = ur.arg);
            mark_self_alive(uth);
            a6_state_entry(uth);
            uth->k.entrance(uth->k.arg);
        }
    }
#endif
    a6i_mark_dying(uth);
    struct a6_uthread *limbo = current_limbo();
    a6_uthread_switch(limbo, uth);
}

// We trust users. That is, customized reincarnation management would be done without mismatched stack sizes;
// As a result of such belief, both init-ruin and create-destroy operations are available for users.

void a6_uthread_init(struct a6_uthread *uth, void (*entrance)(void *), void *arg, void *stkbase, size_t stksize) {
    uth->sched = NULL;
    list_init(&(uth->states));
    (uth->k.cont = NULL), (uth->k.entrance = entrance), (uth->k.arg = arg), (uth->k.stkbase = stkbase), (uth->k.stksize = stksize);
    a6_mcontext_init(&(uth->k.m), a6_uthread_entrance, uth, a6_mcontext_stack_base(stkbase, stksize), stksize);
}

void a6_uthread_ruin(struct a6_uthread *uth) {
    // pretend to do something
}

void a6_uthread_switch(struct a6_uthread *to, struct a6_uthread *from) {
    a6_mcontext_switch(&(to->k.m), &(from->k.m));
}

int a6_uthread_launch(struct a6_uthread *target, struct a6_uthread *cont) {
    if (likely(target->k.cont == NULL)) {
        target->k.cont = cont;
        return a6_mcontext_switch(&(target->k.m), &(cont->k.m)), 1;
    } else
        return 0;
}

// Currently we have no red pages on uthread stacks. Beware of metadata corruption.

#define     A6_PAGE_BYTES                   4096
#define     A6_STACK_ALIGNMENT_BYTES        16

typedef char page_dummy[A6_PAGE_BYTES];

struct a6_uthread *a6_uthread_create(void (*entrance)(void *), void *arg, size_t stknpages) {
    void *stkarea = aligned_alloc(A6_STACK_ALIGNMENT_BYTES, (stknpages + 1) * A6_PAGE_BYTES);
    if (likely(stkarea != NULL)) {
        page_dummy *stkpages = stkarea;
        struct a6_uthread *uth = (void *) stkpages[stknpages];
        return a6_uthread_init(uth, entrance, arg, stkarea, stknpages * A6_PAGE_BYTES), uth;
    } else
        return NULL;
}

void a6_uthread_destroy(struct a6_uthread *uth) {
    a6_uthread_ruin(uth);
    free(uth->k.stkbase);
}

// There is no simple attach/detach operations. They are too complex to be implemented here, in terms of concept purity.
