#include    <core/uthread.h>
#include    <core/scheduler.h>
#include    <core/iomonitor.h>

static __thread struct a6_uthread *curr_uth = NULL;

#define     asynck_trivial      ((struct a6_asynck) { NULL, NULL, NULL })

#define     N_CQUEUES           2
#define     CQUEUE_IORDY        0
#define     CQUEUE_TIMED        1

struct a6_uthread *current_uthread(void) {
    return curr_uth;
}

static
int sched_acquire_qreqs(struct a6_scheduler *sched) {
    return pthread_spin_lock(&(sched->qreqs.lock));
}

static
int sched_release_qreqs(struct a6_iomonitor *iomon, void *sched_p) {
    struct a6_scheduler *sched = sched_p;
    return pthread_spin_unlock(&(sched->qreqs.lock));
}

static
int sched_retimeout(struct a6_iomonitor *iomon, void *sched_p) {
    struct a6_scheduler *sched = sched_p;
    if (!list_is_empty(&(sched->running)))
        iomon->current_state.timeout = 0;
    return 0;
}

int a6_try_acquire_qreqs(struct a6_scheduler *sched) {
    int r = pthread_spin_trylock(&(sched->qreqs.lock));
#ifndef A6_PEDANTIC_ACQUIRE_QREQS
    return r == 0;
#else
    if (r == -1) {
        int errsv = errno;
        return errsv == EBUSY;
    } else
        return likely(r == 0);
#endif
}

struct a6_scheduler *a6_scheduler_init(struct a6_scheduler *sched, uint64_t max_n_uth, struct a6_iomonitor *iomon) {
    list_init(&(sched->running)), list_init(&(sched->blocking)), list_init(&(sched->dying)),
        (sched->baseinfo.max_n_uth = max_n_uth), (sched->iomon = iomon);
    list_init(&(sched->qreqs.queue)), pthread_spin_init(&(sched->qreqs.lock), PTHREAD_PROCESS_PRIVATE);
    a6_evadaptor_init(&(sched->evchan));
    // initialize io extension actions
    {
        // release qreqs
        sched->ioext_hooks[0].arg = sched;
        sched->ioext_hooks[0].hook = sched_release_qreqs;
        a6_attach_ioext_hook(iomon, &(sched->ioext_hooks[0]), IDX_IOEXT_PRETIMED);
        // re-timeout
        sched->ioext_hooks[1].arg = sched;
        sched->ioext_hooks[1].hook = sched_retimeout;
        a6_attach_ioext_hook(iomon, &(sched->ioext_hooks[1]), IDX_IOEXT_PREPOLL);
    }
    return (curr_uth = NULL), sched;
}

struct a6_scheduler *a6_scheduler_ruin(struct a6_scheduler *sched) {
    a6_evadaptor_ruin(&(sched->evchan));
    pthread_spin_destroy(&(sched->qreqs.lock));
    return sched;
}

static void sched_collect(struct a6_ioevent *ev, struct link_index **queues, uint32_t n_queues) {
    // TODO implementation
}

struct a6_scheduler *a6_scheduler_create(uint64_t max_n_uth, struct a6_iomonitor *iomon) {
    struct a6_scheduler *sched = malloc(sizeof(struct a6_scheduler));
    if (unlikely(sched == NULL))
        return NULL;
    return a6_scheduler_init(sched, max_n_uth, iomon);
}

void a6_scheduler_destroy(struct a6_scheduler *sched) {
    free(a6_scheduler_ruin(sched));
}

void schedloop(struct a6_scheduler *s) {
    // TODO implementation
    for (;;) {
        // 0. TODO deliver asynck
        // 1. acquire qreqs & fetch quick requests
        // 2. polling
        // 3. merge requests & rescheduled uthreads into running queue
        // 4. resched / 
    }
}
