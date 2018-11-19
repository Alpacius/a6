#include    <common/tid.h>

static __thread long tid_local = -1;

static uint64_t tseq = 0;

long do_gettid(void) {
    return (tid_local > 0) ? tid_local : (tid_local = syscall(__NR_gettid));
}

uint64_t tseq_next(void) {
    return __atomic_fetch_add(&tseq, 1, __ATOMIC_ACQ_REL);
}
