#include    <core/scheduler.h>
#include    <core/iomonitor.h>

#define     asynck_trivial      ((struct a6_asynck) { NULL, NULL, NULL })

struct a6_scheduler *a6_scheduler_init(struct a6_scheduler *sched, uint64_t max_n_uth, struct a6_iomonitor *iomon) {
    // TODO implementation
    return sched;
}

struct a6_scheduler *a6_scheduler_ruin(struct a6_scheduler *sched) {
    // TODO implementation
    return sched;
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

struct a6_asynck schedloop(struct a6_scheduler *s) {
    struct a6_asynck k = asynck_trivial;
    
    // TODO implementation
    
    // scheduler shall not handle io events

    for (;;) {
    }

    return k;
}
