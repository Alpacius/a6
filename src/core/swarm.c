#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <core/swarm.h>
#include    <core/scheduler.h>
#include    <core/iomonitor.h>
#include    <core/iotrigger.h>
#include    <core/mthread.h>
#include    <core/uthread.h>
#include    <core/uthread_infest.h>

struct a6_swarm {
    struct {
        uint32_t idx, size;
    } slb;
    struct a6_mthread_pool *mthpool;
};

static
int a6_swarm_init(struct a6_swarm *swarm, uint32_t size) {
    swarm->mthpool = a6_mthread_pool_create(size);
    if (unlikely(swarm->mthpool == NULL))
        return 0;
    return (swarm->slb.idx = 0), (swarm->slb.size = size), 1;
}

struct a6_swarm *a6_swarm_create(uint32_t size) {
    struct a6_swarm *swarm = malloc(sizeof(struct a6_swarm));
    if (unlikely(swarm == NULL))
        return NULL;
    return 
        unlikely(a6_swarm_init(swarm, size) == 0) ? 
            (free(swarm), NULL) : 
            swarm;
}

static
struct a6_swarm *a6_swarm_ruin(struct a6_swarm *swarm) {
    return a6_mthread_pool_destroy(swarm->mthpool), swarm;
}

void a6_swarm_destroy(struct a6_swarm *swarm) {
    free(a6_swarm_ruin(swarm));
}

int a6_swarm_launch(struct a6_swarm *swarm) {
    return a6_mthread_pool_launch(swarm->mthpool);
}

int a6_swarm_run(struct a6_swarm *swarm, void (*func)(void *), void *arg) {
    uint32_t idx = __atomic_fetch_add(&(swarm->slb.idx), 1, __ATOMIC_ACQ_REL) % swarm->slb.size;
    return a6_send_uthread_request(scheduler_at(swarm->mthpool, idx), func, arg);
}

int a6_read_barrier_simple(int fd, uint32_t options) {
    struct a6_uthread *uth = current_uthread();
    return a6_simple_read(uth, uth->sched->iomon, fd, options);
}

int a6_write_barrier_simple(int fd, uint32_t options) {
    struct a6_uthread *uth = current_uthread();
    return a6_simple_write(uth, uth->sched->iomon, fd, options);
}

int a6_read_barrier_oneshot(int fd, uint32_t options) {
    struct a6_uthread *uth = current_uthread();
    return a6_oneshot_read(uth, uth->sched->iomon, fd, options);
}

int a6_write_barrier_oneshot(int fd, uint32_t options) {
    struct a6_uthread *uth = current_uthread();
    return a6_oneshot_write(uth, uth->sched->iomon, fd, options);
}
