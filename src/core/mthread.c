#include    <core/mthread.h>


static uint32_t seq_g = 0;

#define a6_follower_prepare(m_, p_) \
    ({ \
        __auto_type m__ = (m_); \
        m__->sched = NULL; \
        m__->pool = (p_); \
    })

#define a6_carrier_prepare(m_, s_, p_) \
    ({ \
        __auto_type m__ = (m_); \
        m__->sched = (s_); \
        m__->pool = (p_); \
    })

#define a6_carrier_launch(m_) \
    ({ \
        __auto_type m__ = (m_); \
        a6_mthread_standby(m__); \
        a6_mthread_launch(m__); \
    })

#define ptok_nil ((uintptr_t) NULL)

#define a6_packed_word_init(w_) \
    ({ \
        struct a6_packed_word *w__ = (w_); \
        (w__->tok = ptok_nil), (w__->mth = NULL); \
    })

#define a6_packed_word_acquire(w_, t_) \
    ({ \
        struct a6_packed_word *w__ = (w_); \
        uintptr_t t__ = (t_); \
        uint32_t d__ = -1; \
        __atomic_compare_exchange_n(&(w__->tok), &d__, t__, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED); \
    })

struct a6_prom_queue *a6_prom_queue_init(struct a6_prom_queue *q, uint32_t size) {
    if (unlikely((q->q = malloc(sizeof(struct a6_packed_word *))) == NULL))
        return NULL;
    for (uint32_t i = 0; i < size; i++)
        a6_packed_word_init(&(q->q[i]));
    return sem_init(&(q->sem_rq_l), 0, 0), sem_init(&(q->sem_rq_f), 0, size), q->size = size, q;
}

struct a6_prom_queue *a6_prom_queue_ruin(struct a6_prom_queue *q) {
    a6_packed_word_acquire(&(q->q[0]), 0);
    return free(q->q), q;
}

struct a6_prom_queue *a6_prom_queue_create(uint32_t size) {
    struct a6_prom_queue *q = malloc(sizeof(struct a6_prom_queue));
    return likely(a6_prom_queue_init(q, size) != NULL) ? q : (free(q), NULL);
}

void a6_prom_queue_destroy(struct a6_prom_queue *q) {
    free(a6_prom_queue_ruin(q));
}

int a6_prom_pick(struct a6_mthread *carrier, struct a6_prom_queue *q) {
    // TODO implementation
    return -1;
}

int a6_prom_wait(struct a6_mthread *follower, struct a6_prom_queue *q) {
    // TODO implementation
    return -1;
}

static
int a6_mthread_wait_promotion(struct a6_mthread *self) {
    // self->sched != NULL -> initial carriers
    if (unlikely(self->sched != NULL))
        return 1;
    struct a6_mthread_pool *pool = self->pool;
    // TODO implementation
    return 1;
}

static
int a6_mthread_unpark(struct a6_asynck k) {
    // TODO implementation
    return 1;
}

static
void *a6_mthread_lifecycle(void *arg) {
    struct a6_mthread *self = arg;
    for (;;) {
        if (unlikely(a6_mthread_wait_promotion(self)) < 1)
            continue;
        struct a6_asynck k = schedloop(self->sched);
        a6_mthread_unpark(k);
    }
    return NULL;
}

static
struct a6_mthread *a6_mthread_init(struct a6_mthread *mth, int type) {
    list_init(intrusion_from_ptr(mth)), 
        (mth->type = type), (mth->status = A6_MTH_PRELAUNCH), 
        (mth->sched = NULL), 
        (mth->aux.seq = __atomic_fetch_add(&seq_g, 1, __ATOMIC_RELAXED)),
        sem_init(&(mth->aux.wchan), 0, 0);
    return mth;
}

static
struct a6_mthread *a6_mthread_ruin(struct a6_mthread *mth) {
    return mth;
}

static
struct a6_mthread *a6_mthread_create(int type) {
    struct a6_mthread *mth = malloc(sizeof(struct a6_mthread));
    return likely(mth != NULL) ? a6_mthread_init(mth, type) : NULL;
}

static
void a6_mthread_destroy(struct a6_mthread *mth) {
    free(a6_mthread_ruin(mth));
}

static
int a6_mthread_launch(struct a6_mthread *mth) {
    return (mth->status = A6_MTH_RUNNING), (pthread_create(&(mth->ptid), NULL, a6_mthread_lifecycle, mth) == 0);
}

static
int a6_mthread_standby(struct a6_mthread *mth) {
    return sem_post(&(mth->aux.wchan));
}

struct a6_mthread_pool *a6_mthread_pool_init(struct a6_mthread_pool *pool, uint32_t cap, uint32_t coresize) {
    (pool->size = 0), (pool->cap = cap), (pool->priq.q = malloc(sizeof(struct a5_mthread *) * coresize));
    if (unlikely(pool->priq.q == NULL))
        return NULL;
    if (a6_prom_queue_init(&(pool->priq), coresize) == NULL)
        return free(pool), NULL;
    return list_init(&(pool->mthlist)), (pool->priq.size = coresize), pool;
}

struct a6_mthread_pool *a6_mthread_pool_ruin(struct a6_mthread_pool *pool) {
    // TODO implementation
    return pool;
}

struct a6_mthread_pool *a6_mthread_pool_create(uint32_t cap, uint32_t coresize) {
    struct a6_mthread_pool *pool = malloc(sizeof(struct a6_mthread_pool));
    if (unlikely(pool == NULL))
        return NULL;
    return likely(a6_mthread_pool_init(pool, cap, coresize) != NULL) ? pool : (free(pool), NULL);
}

void a6_mthread_pool_destroy(struct a6_mthread_pool *pool) {
    free(a6_mthread_pool_ruin(pool));
}

int a6_mthread_pool_launch(struct a6_mthread_pool *pool, struct a6_scheduler **sched_set, uint32_t n_sched) {
    if (n_sched != pool->priq.size)
        return 0;
    for (uint32_t i = 0; i < pool->priq.size; i++) {
        struct a6_mthread *c = a6_mthread_create(A6_MTH_CARRIER);
        if (c)
            list_add_tail(intrusion_from_ptr(c), &(pool->mthlist)), a6_carrier_prepare(c, sched_set[i], pool), a6_carrier_launch(c);
    }
    return 1;
}
