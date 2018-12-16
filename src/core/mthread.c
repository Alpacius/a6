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
    uint64_t max_n_uth;
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

struct a6_mthread *a6_mthread_init(struct a6_mthread *mth) {
    mth->iomon = a6_iomonitor_create(DEFAULT_IO_CAP);
    a6_scheduler_init(&(mth->sched), DEFAULT_MAX_N_UTH, mth->iomon);
    return mth;
}

struct a6_mthread *a6_mthread_ruin(struct a6_mthread *mth) {
    return 
        a6_iomonitor_destroy(mth->iomon),
        a6_scheduler_ruin(&(mth->sched)),
        mth;
}

int a6_mthread_launch(struct a6_mthread *mth, struct a6_mthread_pool *pool) {
    return (mth->pool = pool), pthread_create(&(mth->ptid), NULL, mthread_entrance, mth);
}
