#include    <core/uthread_lifespan.h>


// infested TLS ops for uthread state accessing

// NOTE link-time optimization is strongly recommended

static __thread struct a6_uthread *uth_self = NULL;

struct a6_uthread *uthread_self(void) {
    return uth_self;
}

void set_uthread_self(struct a6_uthread *newself) {
    uth_self = newself;
}
