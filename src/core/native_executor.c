#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/miscutils.h>
#include    <common/list.h>
#include    <common/tribuffer.h>

#include    <semaphore.h>

// It hurts.
struct a6i_async_task {
    intrusive;
    void (*func)(void *);
    void *arg;
};

struct a6i_e_queue {
    // TODO status
    sem_t s;
    uint32_t para;      // Actually, there holds queue.para == rod.size.
    uint32_t lbidx;
    struct tribuffer bufs[];
};

struct a6i_executor_rod {
    uint32_t size;      // Actually, there holds pool.size == rod.size.
    sem_t tok;          // Initially, tok == size.
    struct a6i_e_queue *q;
};

#define     A6I_EPOOL_INIT          0
#define     A6I_EPOOL_RUNNING       1
#define     A6I_EPOOL_DYING         2
#define     A6I_EPOLL_RUINED        3

struct a6i_executor_pool {
    int state;
    struct a6i_executor_rod meta;
    pthread_barrier_t barrier_init;
    uint32_t size;
    pthread_t threads[];
};

struct a6i_async_task *a6i_async_task_create(void (*func)(void *), void *arg) {
    struct a6i_async_task *t = malloc(sizeof(struct a6i_async_task));
    return likely(t != NULL) ? ((t->func = func), (t->arg = arg), t) : NULL;
}

void a6i_async_task_destroy(struct a6i_async_task *t) {
    free(t);
}

int a6i_e_queue_init(struct a6i_e_queue *q, uint32_t para) {
    (q->para = para), (q->lbidx = 0);
    if (sem_init(&(q->s), 0, 0) == -1)
        return 0;
    for (uint32_t i = 0; i < para; i++)
        tribuffer_init(&(q->bufs[i]));
    return 1;
}

void a6i_e_queue_ruin(struct a6i_e_queue *q) {
    sem_destroy(&(q->s));
}

struct a6i_e_queue *a6i_e_queue_create(uint32_t para) {
    struct a6i_e_queue *q = malloc(sizeof(struct a6i_e_queue) + para * sizeof(struct tribuffer));
    if (unlikely(q == NULL))
        return NULL;
    return a6i_e_queue_init(q, para) ? q : (free(q), NULL);
}

void a6i_e_queue_destroy(struct a6i_e_queue *q) {
    a6i_e_queue_ruin(q);
    free(q);
}

int a6i_e_queue_offer(struct a6i_e_queue *q, uint32_t tok, struct a6i_async_task *t) {
    uint32_t idx = tok % q->para;
    // TODO unlikely cancellation check
    struct link_index *wrside = tribuffer_wrside(&(q->bufs[idx]));
    list_add_tail(intrusion_from_ptr(t), wrside);
    int r = sem_post(&(q->s));
    if (r == -1)
        list_del(intrusion_from_ptr(t));
    return r == 0;
}

static inline
struct a6i_async_task *a6i_e_queue_poll_single(struct a6i_e_queue *q, uint32_t idx) {
    struct link_index *rdside = tribuffer_rdside(&(q->bufs[idx]));
    if (list_is_empty(rdside))
        return 0;
    struct link_index *tgt_link = rdside->next;
    list_del(tgt_link);
    return intruded_val(tgt_link, struct a6i_async_task);
}

struct a6i_async_task *a6i_e_queue_poll(struct a6i_e_queue *q) {
    struct a6i_async_task *t = NULL;
    if (sem_wait(&(q->s)) == -1)
        return NULL;
    do {
        for (int i = 0; i < q->para && t == NULL; i++)
            t = a6i_e_queue_poll_single(q, (q->lbidx + i) % q->para);
    } while (t == NULL);
    q->lbidx = (q->lbidx + 1) % q->para;
    return t;
}

int a6i_executor_rod_init(struct a6i_executor_rod *r, uint32_t size) {
    r->size = size;
    return (sem_init(&(r->tok), 0, 1) == 0) && ((r->q = a6i_e_queue_create(size)) != NULL);
}

void a6i_executor_rod_ruin(struct a6i_executor_rod *r) {
    sem_destroy(&(r->tok));
    a6i_e_queue_destroy(r->q);
}

static inline
int a6i_executor_rod_acquire(struct a6i_executor_rod *r) {
    return sem_wait(&(r->tok));
}

static inline
int a6i_executor_rod_release(struct a6i_executor_rod *r) {
    return sem_post(&(r->tok));
}

static
void *worker_loop_lf(void *arg) {
    struct a6i_executor_pool *pool = arg;
    int nouse_cnclstate;
    pthread_barrier_wait(&(pool->barrier_init));
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &nouse_cnclstate);
#define poolstate_p (&(pool->state))
#define pool_alive \
    (__atomic_load_n(poolstate_p, __ATOMIC_ACQUIRE) == A6I_EPOOL_RUNNING)
#define pool_dying \
    (__atomic_load_n(poolstate_p, __ATOMIC_ACQUIRE) == A6I_EPOOL_DYING)
    while (pool_alive) {
        a6i_executor_rod_acquire(&(pool->meta));                        // cancellation point I
        if (unlikely(pool_dying)) {
            a6i_executor_rod_release(&(pool->meta));
            break;
        }
        struct a6i_async_task *t = a6i_e_queue_poll(pool->meta.q);      // cancellation point II
        a6i_executor_rod_release(&(pool->meta));
        if (t && t->func) {
            int old_cnclstate;
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_cnclstate);
            t->func(t->arg);
            a6i_async_task_destroy(t);
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_cnclstate);
        }
        pthread_testcancel();                                           // cancellation point III - redundant
    }
#undef pool_alive
#undef pool_dying
#undef poolstate_p
    return NULL;
}
