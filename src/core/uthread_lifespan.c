#include    <core/uthread_lifespan.h>
#include    <core/uthread_infest.h>


// infested TLS ops for uthread state accessing

// NOTE link-time optimization is strongly recommended

struct a6_uthread *uthread_self(void) {
    return current_uthread();
}

