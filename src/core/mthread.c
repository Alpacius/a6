#include    <core/mthread.h>
#include    <core/scheduler.h>
#include    <core/iomonitor.h>


struct a6_mthread {
    intrusive;
    pthread_t ptid;
    struct a6_scheduler sched;
    struct a6_iomonitor *iomon;
    struct a6_mthread_pool *pool;
    struct {
        void *(*func)(void *);
        void *arg;
    } entrance;
};

struct a6_mthread_pool {
    uint32_t size;
    struct link_index mthq;
    struct a6_mthread mths[];
};
