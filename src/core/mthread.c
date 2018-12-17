#include    <core/mthread.h>
#include    <core/scheduler.h>
#include    <core/iomonitor.h>

struct a6_mthread {
    intrusive;
    pthread_t ptid;
    struct a6_scheduler sched;
    struct a6_iomonitor *iomon;
    struct a6_mthread_pool *pool;
};

struct a6_mthread_pool {
    uint32_t size;
    struct link_index mthq;
    pthread_barrier_t init_barrier;
    struct a6_mthread mths[];
};

static 
void *mthread_entrance(void *arg) {
    struct a6_mthread *self = arg;
    pthread_barrier_wait(&(self->pool->init_barrier));
    schedloop(&(self->sched));
    return NULL;
}

#define     DEFAULT_IO_CAP          1024
#define     DEFAULT_MAX_N_UTH       8192

int a6_mthread_init(struct a6_mthread *mth, struct a6_mthread_pool *pool) {
    mth->iomon = a6_iomonitor_create(DEFAULT_IO_CAP);
    a6_scheduler_init(&(mth->sched), DEFAULT_MAX_N_UTH, mth->iomon);
    return (mth->pool = pool), 1;
}

struct a6_mthread *a6_mthread_ruin(struct a6_mthread *mth) {
    return 
        a6_iomonitor_destroy(mth->iomon),
        a6_scheduler_ruin(&(mth->sched)),
        mth;
}

int a6_mthread_launch(struct a6_mthread *mth) {
    return pthread_create(&(mth->ptid), NULL, mthread_entrance, mth);
}

int a6_mthread_pool_init(struct a6_mthread_pool *pool) {
    list_init(&(pool->mthq));
    pthread_barrier_init(&(pool->init_barrier), NULL, pool->size);
    for (uint32_t i = 0; i < pool->size; i++) {
        list_add_tail(intrusion_from_ptr(&(pool->mths[i])), &(pool->mthq));
        a6_mthread_init(&(pool->mths[i]), pool);
    }
    return 1;
}

void a6_mthread_pool_ruin(struct a6_mthread_pool *pool) {
    list_init(&(pool->mthq));
    for (uint32_t i = 0; i < pool->size; i++)
        a6_mthread_ruin(&(pool->mths[i]));
    pthread_barrier_destroy(&(pool->init_barrier));
}

int a6_mthread_pool_launch(struct a6_mthread_pool *pool) {
    for (uint32_t i = 0; i < pool->size; i++)
        a6_mthread_launch(&(pool->mths[i]));
    return 1;
}
