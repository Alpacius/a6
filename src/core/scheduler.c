#include    <core/uthread.h>
#include    <core/scheduler.h>
#include    <core/iomonitor.h>

static __thread struct a6_uthread *curr_uth = NULL;
static __thread struct a6_uthread *curr_limbo = NULL;
static __thread struct a6_scheduler *curr_sched = NULL;

#define     asynck_trivial      ((struct a6_asynck) { NULL, NULL, NULL })

#define     N_CQUEUES           3
#define     CQUEUE_IORDY        0
#define     CQUEUE_TIMED        1
#define     CQUEUE_CREAT        2

struct a6_uthread *current_uthread(void) {
    return curr_uth;
}

struct a6_uthread *current_limbo(void) {
    return curr_limbo;
}

static inline
int sched_acquire_qreqs(struct a6_scheduler *sched) {
    return pthread_spin_lock(&(sched->qreqs.lock));
}

static inline
int sched_release_qreqs(struct a6_iomonitor *iomon, void *sched_p) {
    struct a6_scheduler *sched = sched_p;
    return pthread_spin_unlock(&(sched->qreqs.lock));
}

static inline
int sched_retimeout(struct a6_iomonitor *iomon, void *sched_p) {
    struct a6_scheduler *sched = sched_p;
    if (!list_is_empty(&(sched->running)))
        iomon->current_state.timeout = 0;
    return 0;
}

static inline
struct a6_uthread *uth_from_req(struct a6_uth_req *req) {
    struct a6_uthread *uth = a6_uthread_create(req->func, req->arg, DEFAULT_N_STKPAGES);
    if (req->dispose)
        req->dispose(req);
    return uth;
}

static
void uth_req_dispose_glibc(struct a6_uth_req *r) {
    free(r);
}

int a6_send_uthread_request(struct a6_scheduler *sched, void (*func)(void *), void *arg) {
    if (pthread_spin_trylock(&(sched->qreqs.lock)) == 0) {
        struct a6_uth_req *rp = malloc(sizeof(struct a6_uth_req));
        if (unlikely(rp == NULL))
            return 0;
        (rp->func = func), (rp->arg = arg), (rp->dispose = uth_req_dispose_glibc);
        list_add_tail(intrusion_from_ptr(rp), &(sched->qreqs.queue));
        pthread_spin_unlock(&(sched->qreqs.lock));
    } else {
        struct a6_uth_req ri = { .func = func, .arg = arg, .dispose = NULL };
        return (write(a6_evadaptor_write_end(&(sched->evchan)), &ri, sizeof(ri)) == sizeof(ri));
    }
    return 1;
}

int a6_try_acquire_qreqs(struct a6_scheduler *sched) {
    return pthread_spin_trylock(&(sched->qreqs.lock)) == 0;
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

static
void sched_collect(struct a6_ioevent *ev, struct link_index **queues, uint32_t n_queues) {
    struct a6_waitk *k = ev->udata;
    switch (k->type) {
        case A6_WAITK_DUMMY:
            {
                struct a6_uth_req req;
                ssize_t rdsize = 0;
                do {
                    rdsize = read(a6_evadaptor_read_end(&(curr_sched->evchan)), &req, sizeof(req));
                    if (likely(rdsize == sizeof(struct a6_uth_req))) {
                        struct a6_uthread *uth_new = uth_from_req(&req);
                        if (likely(uth_new != NULL))
                            list_add_tail(intrusion_from_ptr(uth_new), queues[CQUEUE_CREAT]);
                    }
                } while (rdsize > 0);
                break;
            }
        // TODO timer events
        default:
            list_add_tail(intrusion_from_ptr(k->uth), queues[CQUEUE_IORDY]);
    }
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
    struct link_index qreqs;
    struct link_index pollables[N_CQUEUES];
    struct a6_uthread sched_cntx;
    curr_limbo = &sched_cntx;
    curr_sched = s;
    for (;;) {
        // 0. TODO deliver asynck
        // 1. acquire qreqs & fetch quick requests
        list_init(&qreqs);
        sched_acquire_qreqs(s);
        {
            list_move(&qreqs, &(s->qreqs.queue));
            // 2. polling
            struct link_index *pollables_p = pollables;     // Slience!
            a6_iomonitor_poll(s->iomon, &pollables_p, N_CQUEUES, sched_collect, 0);
        }
        // 3. merge requests & rescheduled uthreads into running queue
        for (int i = 0; i < N_CQUEUES; i++)
            list_foreach_remove(&(pollables[i])) {
                detach_current_iterator;
                list_add_tail(iterator, &(s->running));
            }
        // TODO acuqire qreqs again for better slb performance
        list_foreach_remove(&qreqs) {
            detach_current_iterator;
            struct a6_uth_req *req = intrusive_ref(struct a6_uth_req);
            struct a6_uthread *uth_new = uth_from_req(req);
            if (likely(uth_new != NULL))
                list_add_tail(intrusion_from_ptr(uth_new), &(s->running));
        }
        // 4. resched
        if (likely(list_is_empty(&(s->running)) == 0)) {
            struct link_index *target = s->running.next;
            list_del(target);
            struct a6_uthread *uth_next = intruded_val(target, struct a6_uthread);
            curr_uth = uth_next;
            curr_uth->sched = s;
            a6_uthread_switch(uth_next, &sched_cntx);
        }
    }
}
