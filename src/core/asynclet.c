#include    <common/miscutils.h>
#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <core/asynclet.h>
#include    <core/promise_internal.h>
#include    <core/waitk.h>
#include    <core/evslot_k.h>
#include    <core/uthreq.h>
#include    <core/uthread.h>
#include    <core/uthread_lifespan.h>
#include    <core/uthread_infest.h>
#include    <core/scheduler.h>
#include    <core/iomonitor.h>
#include    <core/evadaptor_afunix.h>
#include    <core/async_utils.h>

struct a6_uthread *current_uthread(void);
void a6i_ioev_attach_k(struct a6_waitk *k);

static
int a6i_prepare_crchan_read(struct a6_iomonitor *iomon, struct a6_uthread *uth, struct a6_waitk *udata) {
    struct epoll_event ev;
    ev.events = EPOLLIN|EPOLLONESHOT;
    ev.data.fd = a6_evadaptor_read_end(&(uth->sched->evchan));
    a6i_ioev_attach_k(udata);
    // NOTE evchan is already attached to epoll - no need to register again
    return 1;
}

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

int a6_promise_init_(struct a6_promise *p, size_t extsz) {
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
    int r = 1, echan = A6_ASYNC_CHAN_INIT;
    struct a6_uthread *uth = current_uthread();
    f->uth = uth;
    struct a6_waitk k;
    k.type = A6_WAITK_PLAIN;
    k.fd.i = a6_evadaptor_read_end(&(uth->sched->evchan));
    k.uth = current_uthread();
    if (!__atomic_compare_exchange_n(
                &(f->base.chan), &echan, a6_evadaptor_write_end(&(uth->sched->evchan)), 
                0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)) {
        while (__atomic_load_n(&(f->base.chan), __ATOMIC_CONSUME) != A6_ASYNC_CHAN_Q_PH2)
            cpu_relax;
    } else {
        r = a6i_prepare_crchan_read(uth->sched->iomon, uth, &k);
        a6_uth_blocking;
        uthread_yield;
    }
    return r;
#undef      cpu_relax
}

static inline void do_fill(struct a6_future *f, int opcode, va_list ap);

int a6_promise_put_(struct a6_promise *p, int opcode, ...) {
    int r = 1, echan = A6_ASYNC_CHAN_INIT;
    {
        va_list ap;
        va_start(ap, opcode);
        do_fill(p->future, opcode, ap);
        va_end(ap);
    }
    if (__atomic_compare_exchange_n(&(p->future->base.chan), &echan, A6_ASYNC_CHAN_Q_PH1, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)) {
        __atomic_store_n(&(p->future->base.chan), A6_ASYNC_CHAN_Q_PH2, __ATOMIC_RELEASE);
    } else {
        struct a6_async_req rload = { .arg = p->future };
        struct a6_req_packet rpkt = { .type = A6_REQ_TYPE_ASYNC };
        rpkt.payload.r_async = rload;
        r = write(__atomic_load_n(&(p->future->base.chan), __ATOMIC_ACQUIRE), &rpkt, sizeof(rpkt)) != -1;
    }
    return r;
}

static inline
void do_fill(struct a6_future *f, int opcode, va_list ap) {
    switch (opcode) {
        case A6_ASYNC_RV_PWORD:
            {
                uintptr_t pword = va_arg(ap, uintptr_t);
                f->val_pword = pword;
            }
            break;
        case A6_ASYNC_RV_U64:
            {
                uint64_t u64 = va_arg(ap, uint64_t);
                f->val_u64 = u64;
            }
            break;
        case A6_ASYNC_RV_U32:
            {
                uint32_t u32 = va_arg(ap, uint32_t);
                f->val_u32 = u32;
            }
            break;
        case A6_ASYNC_RV_I64:
            {
                int64_t i64 = va_arg(ap, int64_t);
                f->val_i64 = i64;
            }
            break;
        case A6_ASYNC_RV_I32:
            {
                int32_t i32 = va_arg(ap, int32_t);
                f->val_i32 = i32;
            }
            break;
        case A6_ASYNC_RV_INT:
            {
                int iv = va_arg(ap, int);
                f->val_int = iv;
            }
            break;
        case A6_ASYNC_RV_UDS:
            {
                void *udsv = va_arg(ap, void *);
                size_t sz = va_arg(ap, size_t);
                f->val_ptr = udsv;
                f->base.valsize = sz;
                // NOTE currently swallow copy is not enabled
            }
            break;
        default:
            ;
    }
}
