#pragma once

#include    <common/stdc_common.h>
#include    <common/miscutils.h>

struct a6_queue {
    int (*shift)(struct a6_queue *, void *);
    void *(*poll)(struct a6_queue *);
    void *(*peek)(const struct a6_queue *);
    int (*dtor)(struct a6_queue *);
};

#define     a6_queue_stub_              a6qctl_
#define     a6_queue_host               struct a6_queue a6_queue_stub_
#define     a6_queue_from_imm(i_)       (&((i_).a6_queue_stub_))
#define     a6_queue_from_ptr(p_)       (&((p_)->a6_queue_stub_))
#define     a6_queue_impl(o_, t_)       container_of((o_), t_, a6_queue_stub_)

#define     a6_queue_inv_(c_, o_, ...) \
    ({ \
        __auto_type o__ = (o_); \
        o__->c_(o__, ##_VA_ARGS__); \
    })

#ifdef      A6_IFCE_METHOD_PROT

static inline
int a6_queue_shift(struct a6_queue *q, void *e) {
    return q->shift(q, e);
}

static inline
void *a6_queue_poll(struct a6_queue *q) {
    return q->poll(q);
}

static inline
void *a6_queue_peek(const struct a6_queue *q) {
    return q->peek(q);
}

static inline
int a6_queue_destroy(struct a6_queue *q) {
    return q->dtor(q);
}

#else

#define     a6_queue_shift(q_, e_)      a6_queue_inv_(shift, (q_), (e_))
#define     a6_queue_poll(q_)           a6_queue_inv_(poll, (q_))
#define     a6_queue_peek(q_)           a6_queue_inv_(peek, (q_))
#define     a6_queue_destroy(q_)        a6_queue_inv_(dtor, (q_))

#endif      // A6_IFCE_METHOD_PROT
