#include    <common/miscutils.h>
#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <core/asynclet.h>
#include    <core/promise_internal.h>
#include    <core/uthread.h>
#include    <core/scheduler.h>
#include    <core/iomonitor.h>
#include    <core/evadaptor_afunix.h>

struct a6_uthread *current_uthread(void);

static inline
int a6_future_prepare(struct a6_future *f, struct a6_promise *p) {
    __atomic_store_n(&(f->base.chan), A6_ASYNC_CHAN_INIT, __ATOMIC_RELEASE);
    (f->uth = NULL), (f->promise = p);
    return 1;
}

struct a6_future *a6_future_create(struct a6_promise *p, size_t extsz) {
    struct a6_future *f = malloc(sizeof(struct a6_future) + extsz);
    if (unlikely(f == NULL))
        return NULL;
    return likely(a6_future_prepare(f, p)) ? f : (free(f), NULL);
}

// No ruin op for futures. Just destroy them.

void a6_future_destroy(struct a6_future *f) {
    free(f);
}

int a6_promise_init(struct a6_promise *p, size_t extsz) {
    return (p->future = a6_future_create(p, extsz)) == NULL;
}

void a6_promise_ruin(struct a6_promise *p) {
    a6_future_destroy(p->future);
}

int a6_future_wait(struct a6_future *f) {
#ifdef      __x86_64__
#define     cpu_relax       __builtin_ia32_pause()
#else
#define     cpu_relax
#endif
    int echan = A6_ASYNC_CHAN_INIT;
    if (!__atomic_compare_exchange_n(
                &(f->base.chan), &echan, a6_evadaptor_write_end(&(current_uthread()->sched->evchan)), 
                0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)) {
        // spinning for q.ph2
        while (__atomic_load_n(&(f->base.chan), __ATOMIC_CONSUME) != A6_ASYNC_CHAN_Q_PH2)
            cpu_relax;
    } else {
        // TODO blocking for arrival of async packet on cr channel
    }
    return 1;
#undef      cpu_relax
}

static inline void do_fill(struct a6_future *f, int opcode, va_list ap);

int a6_promise_put(struct a6_promise *p, int opcode, ...) {
    int echan = A6_ASYNC_CHAN_INIT;
    if (__atomic_compare_exchange_n(&(p->future->base.chan), &echan, A6_ASYNC_CHAN_Q_PH1, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)) {
        {
            va_list ap;
            va_start(ap, opcode);
            do_fill(p->future, opcode, ap);
            va_end(ap);
        }
        __atomic_store_n(&(p->future->base.chan), A6_ASYNC_CHAN_Q_PH2, __ATOMIC_RELEASE);
    } else {
        // TODO write async packet to p->future->base.chan, i.e. cr channel
    }
    return 1;
}

static inline
void do_fill(struct a6_future *f, int opcode, va_list ap) {
    // TODO implementation
}
