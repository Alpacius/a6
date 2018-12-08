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

struct a6_scheduler *a6_scheduler_init(struct a6_scheduler *sched, uint64_t max_n_uth, struct a6_iomonitor *iomon) {
    list_init(&(sched->running)), list_init(&(sched->blocking)), list_init(&(sched->dying)),
        (sched->baseinfo.max_n_uth = max_n_uth), (sched->iomon = iomon);
    list_init(&(sched->qreqs.queue)), pthread_spin_init(&(sched->qreqs.lock), PTHREAD_PROCESS_PRIVATE);
    a6_evadaptor_init(&(sched->evchan));
    return (curr_uth = NULL), sched;
}

struct a6_scheduler *a6_scheduler_ruin(struct a6_scheduler *sched) {
    a6_evadaptor_ruin(&(sched->evchan));
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

struct a6_asynck schedloop(struct a6_scheduler *s) {
    struct a6_asynck k = asynck_trivial;
    
    // TODO implementation
    
    for (;;) {
    }

    return k;
}
